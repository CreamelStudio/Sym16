# Sym16

Sym16은 C99용 헤더 전용 라이브러리입니다. 일반 `char` 대신 16비트
`Symbol` 타입을 사용해서 문자의 종류와 값을 한 번에 저장합니다.

```text
[ 상위 4비트: Type ][ 하위 12비트: Value ]
```

`.c` 파일이 따로 필요 없고, `malloc`을 사용하지 않으며, 핵심 헤더는
`stdint.h`, `stdbool.h`, `stddef.h`만 사용합니다.

## 빠른 시작

```c
#include "sym16.h"

Symbol out[32];
size_t len = sym_encode("abc123+", out, 32);
```

디버그 출력 예시:

```text
[LOWER:a]
[LOWER:b]
[LOWER:c]
[DIGIT:1]
[DIGIT:2]
[DIGIT:3]
[OP:+]
```

예제 빌드:

```sh
cc -std=c99 -Wall -Wextra -pedantic main.c -o sym16_example
./sym16_example
```

## 헤더 선택

한 `.c` 파일에서는 아래 셋 중 하나만 include하세요.

```c
/* 하나만 선택하세요. 셋 다 include하면 안 됩니다. */
#include "sym16.h"          /* 기본 Simple 버전 */
/* #include "sym16_fast.h" */    /* ASCII 인코딩 빠른 버전 */
/* #include "sym16_compact.h" */ /* value 압축 버전 */
```

세 헤더 모두 공개 API 이름은 같습니다. 즉 어떤 헤더를 골라도
`sym_encode`, `sym_decode`, `sym_type`, `sym_value` 같은 함수를 그대로
사용합니다.

| 헤더 | Value 의미 | 추천 상황 |
| --- | --- | --- |
| `sym16.h` | 원래 byte 값 저장. 예: `'a' == 97` | 학습, 디버깅, 문자열 복원, 가장 안전한 기본값 |
| `sym16_fast.h` | `sym16.h`와 같지만 ASCII 분류에 lookup table 사용 | ASCII 중심 입력을 많이, 자주 인코딩할 때 |
| `sym16_compact.h` | 타입 내부 인덱스 저장. 예: `'7' == 7`, `'c' == 2`, `'+' == 0` | Lexer/Parser에서 숫자값, 알파벳 인덱스, 연산자 인덱스를 바로 쓰고 싶을 때 |

## 메모리 구조

`Symbol`은 `uint16_t`입니다.

```text
15            12 11                         0
+---------------+---------------------------+
| Type, 4 bits   | Value, 12 bits            |
+---------------+---------------------------+
```

`sym_make(type, value)`는 type과 value를 각각 마스킹합니다. 따라서 type은
4비트, value는 12비트만 저장됩니다.

기본 버전과 fast 버전에서는 ASCII byte가 그대로 `Value`에 들어갑니다.
compact 버전에서는 항상 원래 ASCII 값이 들어가지는 않습니다. 예를 들어
`'a'`는 97이 아니라 lower 알파벳의 0번째 값으로 저장됩니다. 그래도
지원되는 ASCII 입력에 대해서는 `sym_decode`와 `sym_dump`가 보이는 문자를
다시 복원합니다.

## API

- `sym_make(type, value)`
- `sym_type(symbol)`
- `sym_value(symbol)`
- `sym_is_digit(symbol)`
- `sym_is_upper(symbol)`
- `sym_is_lower(symbol)`
- `sym_is_space(symbol)`
- `sym_is_operator(symbol)`
- `sym_encode(input, output, max_output)`
- `sym_decode(input, length, output, max_output)`
- `sym_compare(a, len_a, b, len_b)`
- `sym_find(data, len, target)`
- `sym_type_name(type)`
- `sym_format(symbol, output, max_output)`
- `sym_dump(input, length, output, max_output)`

## 어디에서 유리한가

Sym16은 단순 문자열 저장용으로는 보통 손해입니다.

```text
char   = 1바이트
Symbol = 2바이트
```

텍스트를 한 번 읽고 출력하거나, 파일을 복사하거나, 문자열 저장만 한다면
그냥 C 문자열이 더 낫습니다.

Sym16이 유리해지는 지점은 문자를 한 번 읽은 뒤 계속해서 이런 질문을 반복할
때입니다.

```c
이 문자가 숫자인가?
대문자인가?
소문자인가?
공백인가?
연산자인가?
```

잘 맞는 곳:

- Lexer와 Parser
- 작은 DSL 인터프리터
- 문법 하이라이터
- 입력 검증기
- 같은 입력 버퍼를 여러 번 스캔하는 도구
- `malloc` 없이 고정 크기 배열로 처리해야 하는 임베디드/저수준 코드
- `isdigit`, `isalpha` 같은 분류를 여러 단계에서 반복하는 코드

잘 맞지 않는 곳:

- 일반 문자열 저장
- 한 번만 읽고 끝나는 텍스트 처리
- 파일 복사, 단순 출력
- 완전한 Unicode 문자열 표현
- ASCII 저장 공간이 2배가 되는 것이 부담인 환경

## 성능 분석

핵심 연산은 비트 시프트와 마스크입니다.

- `sym_make`, `sym_type`, `sym_value`: O(1)
- 타입 검사: O(1)
- encode/decode: O(n), 한 번 선형 순회
- compare: O(min(n, m))
- find: O(n)

모든 주요 함수는 `static inline`입니다. 최적화 컴파일러는 보통 작은 함수 호출
오버헤드를 제거할 수 있습니다.

프로필별 차이:

| 프로필 | 인코딩 비용 | 메모리 비용 | 특징 |
| --- | --- | --- | --- |
| Simple | byte마다 예측 가능한 분기 몇 번 | Symbol당 2바이트 | value가 원래 byte라 디버깅이 가장 쉬움 |
| Fast | lookup table 조회 후 fallback | Symbol당 2바이트 + `.c` 파일마다 작은 static table | ASCII-heavy 입력을 많이 인코딩할 때 유리 |
| Compact | 분기 후 value remapping | Symbol당 2바이트 | 이후 로직에서 숫자값/알파벳 인덱스/연산자 인덱스를 바로 쓰기 좋음 |

fast 버전은 인코딩 시 분기 수를 줄일 수 있지만, include한 translation unit마다
256개짜리 `Symbol` lookup table을 가집니다. 대략 512바이트 정도입니다.

## 확장 방법

UTF-8은 두 가지 방향으로 확장할 수 있습니다.

- byte-preserving 방식: UTF-8 byte 하나를 Symbol 하나로 저장합니다.
- codepoint 방식: UTF-8을 먼저 Unicode codepoint로 디코딩한 뒤 Symbol 하나
  또는 여러 개로 매핑합니다.

한글 완성형 음절 전체를 12비트 value에 직접 넣기는 어렵습니다. 현대 한글
음절은 11,172개라 12비트 4,096개 공간보다 큽니다.

현실적인 방법:

- `SYM_HANGUL`에 초성/중성/종성 jamo 인덱스를 저장합니다.
- 한 Unicode 글자를 여러 Symbol로 나눠 저장합니다.
- 외부 테이블을 두고 12비트 value에는 테이블 인덱스를 저장합니다.

Lexer와 연동하려면 먼저 입력 문자열을 `Symbol` 배열로 인코딩한 뒤,
토큰 규칙을 `sym_type()`과 `sym_value()` 기준으로 작성하면 됩니다. 연산자는
이미 `SYM_OPERATOR` 타입으로 분리되므로 간단한 토크나이저를 만들기 좋습니다.

## 예제

```c
#include <stdio.h>
#include "sym16.h"

int main(void)
{
    Symbol symbols[64];
    char dump[512];
    size_t len = sym_encode("abc123+", symbols, 64);

    sym_dump(symbols, len, dump, sizeof(dump));
    puts(dump);

    return 0;
}
```
