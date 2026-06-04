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

그래서 테스트나 프로토타입 단계에서는 기본 `sym16.h`로 시작하면 됩니다.
나중에 용도에 맞는 프로필이 확실해지면 include 한 줄만 `sym16_fast.h` 또는
`sym16_compact.h`로 바꾸면 됩니다. 함수명은 그대로이고 내부 인코딩 전략만
달라집니다. 단, compact 모드는 `sym_value()`의 숫자 의미를 의도적으로
바꾸므로 raw value에 직접 의존하는 코드는 확인해야 합니다.

| 헤더 | Value 의미 | 추천 상황 |
| --- | --- | --- |
| `sym16.h` | 원래 byte 값 저장. 예: `'a' == 97` | 학습, 디버깅, 문자열 복원, 가장 안전한 기본값 |
| `sym16_fast.h` | `sym16.h`와 같지만 ASCII 분류에 lookup table 사용 | ASCII 중심 입력을 많이, 자주 인코딩할 때 |
| `sym16_compact.h` | 타입 내부 인덱스 저장. 예: `'7' == 7`, `'c' == 2`, `'+' == 0` | Lexer/Parser에서 숫자값, 알파벳 인덱스, 연산자 인덱스를 바로 쓰고 싶을 때 |

## 헤더 선택 가이드

`sym16.h`가 맞는 경우:

- 라이브러리를 처음 써보거나 프로토타입을 만들 때
- 디버깅이 쉬운 구조가 필요할 때
- `sym_value(sym_from_char('a'))`가 원래 byte 값인 `97`이기를 원할 때
- ASCII 텍스트를 가장 덜 놀라운 방식으로 encode/decode하고 싶을 때
- 아직 어떤 프로필을 골라야 할지 모르겠을 때

`sym16_fast.h`가 맞는 경우:

- 작은 static lookup table보다 인코딩 속도가 더 중요할 때
- 입력이 대부분 ASCII일 때
- 문자열이나 큰 버퍼를 반복해서 많이 인코딩할 때
- scanner, validator, preprocessor처럼 입력 분류가 hot path일 때
- `Value`는 `sym16.h`처럼 원래 byte 의미를 유지하고 싶을 때

`sym16_compact.h`가 맞는 경우:

- 이후 로직이 원래 byte보다 의미 있는 작은 값에 관심이 많을 때
- 숫자를 바로 값으로 쓰고 싶을 때. 예: `'7' -> 7`
- 알파벳을 인덱스로 쓰고 싶을 때. 예: `'c' -> 2`
- 연산자를 ID로 쓰고 싶을 때. 예: `'+' -> 0`
- Lexer/Parser 규칙에서 먼저 `Type`으로 분기하고, 그다음 타입 내부의 작은
  `Value`를 쓰고 싶을 때

빠른 예시:

| 만들고 싶은 것 | 추천 헤더 | 이유 |
| --- | --- | --- |
| Sym16 첫 실험 | `sym16.h` | 가장 읽기 쉽고 덜 헷갈림 |
| 디버그 dump 도구 | `sym16.h` | value가 원래 byte와 같음 |
| ASCII 로그 스캐너 | `sym16_fast.h` | 반복 분류가 lookup으로 빨라질 수 있음 |
| 소스 코드 Lexer | `sym16_fast.h` 또는 `sym16_compact.h` | 원시 스캔은 fast, 의미 기반 토큰 규칙은 compact가 유리 |
| 계산기 Parser | `sym16_compact.h` | 숫자와 연산자가 작은 직접 값이 됨 |
| 임베디드 고정 버퍼 Parser | `sym16_compact.h` | heap 없이 예측 가능하고 타입 내부 값이 작음 |
| 일반 텍스트 저장 | 사용하지 말고 `char` | ASCII 저장 공간이 2배가 됨 |
| 완전한 Unicode 텍스트 엔진 | 단독으로는 부족 | UTF-8/codepoint 계층이 먼저 필요 |

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

## 커스텀 심볼

Type `8`부터 `15`까지는 사용자 정의 심볼 영역입니다.

```c
SYM_CUSTOM0
SYM_CUSTOM1
SYM_CUSTOM2
SYM_CUSTOM3
SYM_CUSTOM4
SYM_CUSTOM5
SYM_CUSTOM6
SYM_CUSTOM7
```

입력 문자 하나에서 바로 나온 값이 아니라, 파서가 중간에 끼워 넣는 의미 태그가
필요할 때 쓰면 됩니다. 예를 들면 토큰 마커, 가상 구분자, 매크로 placeholder,
AST 힌트, 도메인 전용 상태 같은 것들입니다.

```c
enum {
    MY_TOKEN_IDENTIFIER = 1,
    MY_TOKEN_NUMBER = 2
};

Symbol id = sym_make_custom(0, MY_TOKEN_IDENTIFIER);
Symbol num = sym_make_custom(0, MY_TOKEN_NUMBER);

if (sym_is_custom(id) && sym_custom_type(id) == 0) {
    /* custom token 처리 */
}
```

중요한 제한:

- `sym_encode()`는 커스텀 심볼을 자동으로 만들지 않습니다.
- `sym_make_custom(custom_type, value)`의 `custom_type`은 `0`부터 `7`까지입니다.
- 잘못된 custom type을 넣으면 `SYM_NULL`을 반환합니다.
- `value`는 여전히 12비트라 `0`부터 `4095`까지만 저장됩니다.
- 커스텀 심볼은 텍스트가 아니므로 `sym_decode()`에서는 `?`로 나옵니다.
- `sym_dump()`는 커스텀 값을 hex로 출력합니다. 예: `[CUSTOM0:0x0001]`

이건 일부러 registry 시스템으로 만들지 않았습니다. 전역 이름 테이블, callback,
동적 할당이 없습니다. 커스텀 value에 이름이 필요하다면 Lexer/Parser 계층에서
따로 관리하는 편이 낫습니다.

## API

- `sym_make(type, value)`
- `sym_make_custom(custom_type, value)`
- `sym_type(symbol)`
- `sym_value(symbol)`
- `sym_is_custom(symbol)`
- `sym_is_custom_type(custom_type)`
- `sym_custom_type(symbol)`
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
