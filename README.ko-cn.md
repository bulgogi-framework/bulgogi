<div align="center" style="margin-top: 20px; margin-bottom: -30px;">
  <img src="https://raw.githubusercontent.com/bulgogi-framework/.github/main/res/img/Bulgogi.svg" alt="bulgogi logo" style="max-width: 100%; max-height: 1024px;">
</div>

<h1 align="center">Bulgogi (불고기)</h1>

<p align="center">
  <b>Boost.Beast 기반의 가벼운 C++ HTTP API framework</b>
</p>

<p align="center">
  <a href="https://github.com/bulgogi-framework/bulgogi/blob/main/docs/quick_start.md">
    <img src="https://img.shields.io/badge/Quick%20Start-black?style=for-the-badge&logo=fastapi" alt="빠른 시작"/>
  </a>
</p>

<p align="center">
  <a href="https://github.com/bulgogi-framework/bulgogi#readme">
    <img src="https://img.shields.io/badge/Back%20to%20English%20README-black?style=for-the-badge&logo=github" alt="Back to README"/>
  </a>
</p>

---

<div align="center">

[![C++20](https://img.shields.io/badge/C%2B%2B-20-violet.svg)](https://en.cppreference.com/w/cpp/20)
[![Boost](https://img.shields.io/badge/Boost-1.88-blue.svg)](https://www.boost.org/)
[![jh-toolkit](https://img.shields.io/badge/jh--toolkit-1.3.x--LTS-brightgreen)](https://github.com/JeongHan-Bae/JH-Toolkit/tree/1.3.x-LTS)
[![CMake](https://img.shields.io/badge/CMake-3.25%2B-blue)](https://cmake.org/)
[![Clang](https://img.shields.io/badge/Clang-14.0%2B-yellow)](https://clang.llvm.org/)
[![Docker](https://img.shields.io/badge/Docker-Buildx-lightgrey)](https://docs.docker.com/buildx/working-with-buildx/)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Contributors](https://img.shields.io/github/contributors/bulgogi-framework/bulgogi.svg)](https://github.com/bulgogi-framework/bulgogi/graphs/contributors)

</div>

_이 문서는 주로 중국 지역의 조선족 독자를 위해 '우리말' 기준 표현을 중심으로 작성되었습니다._  
_서울말 사용자에게는 일부 어휘나 표기가 익숙하지 않을 수 있으며,_  
_특히 기술 관련 외래어는 음역 없이 원형 그대로 표기하는 것이 원칙입니다._  
_이는 의미 전달을 명확히 하고, 중국 사용자에게 더 자연스러운 문맥을 제공하기 위함입니다._


## 🔥 "Bulgogi"란?

**Bulgogi (불고기)** 는 Boost.Beast 위에 구축된 C++ HTTP API framework 로,  
불필요한 의존(dependency) 항목(project)과 복잡도(complexity)를 줄이고, 개발자 에게 높은 제어력과 빠른 반복 작업 환경을 제공합니다.

다음과 같은 분야에 적합합니다:

* Backend service API
* 내부 도구(道具) 개발
* RPC style server
* Embedded 환경 또는 Game server
* JS frontend 또는 CLI client 련동함

---

## ✨ 핵심 기능

* ⚡️ **한 macro = 한 route**: API 개발 방식이 간단하고, 작성 효률 높음  
* 🧩 **기본적으로 web layer 를 포함하지 않음**: template engine 과 frontend 가 전제로 깔려 있지 않음
* 🐚 **runtime 복잡도 없음**: coroutine, framework, 과도한 추상(抽象) 없음  
* 🐳 **Docker 사용 가능**: 개발용과 배포용 image 제공  
* 💡 **Debug 상태에서만 root page 표시됨**: 순수 API 환경에 적합 
* 🛠 **Boost.Beast** 및 **jh-toolkit** 기반의 설계

---

## 🧩 Web Layer 없는 설계

Bulgogi 는 Web layer 를 **의도적으로 제외한** 구조(構造)로 설계되었습니다:

* HTML renderer 및 template DSL 없음  
* 정적 File 지원 없음 (필요 시 직접 구현)  
* API 는 curl, Postman, JS client 또는 내부 system 으로 호출  
* root debug page 는 `NDEBUG` 가 정의되지 않았을 때만 표시됨

---

## 📁 항목 구조

```text
.
├── CMakeLists.txt            # 최상위 build 설정 File
├── main.cpp                  # 진입점 (고칠 필요 없음)
├── Web/
│   ├── views.cpp             # route 처리를 담는 구현부
│   ├── views.hpp             # route macro 선언용 구문 (고칠 부분 없음)
│   ├── template.hpp          # 선택적으로 HTML registry
│   └── ...
├── Application/              # 업무 흐름 구현부
├── Entities/                 # Data 구조 정의 (POD 권장, jh-toolkit 기반)
├── docker/
│   ├── Dockerfile
│   └── Dockerfile.runtime
├── docs/
│   ├── quick_start.md        # 설치 및 초기 사용 안내
│   └── api.md                # route 사용 및 HTTP 설명
├── NOTICE.md                 # Third Party License 목록
├── build.md                  # 상세한 build / 배포 안내
└── LICENSE                   # MIT License
```

---

## 📚 문서 안내

* 📦 [빠른 시작](docs/quick_start.md)
* 🔧 [구건(構建) 및 배포](build.md)
* 📖 [API - 응용 정서(程序) 개면 참조](docs/api.md)

---

## 🍖 이름이 왜 "Bulgogi"인가요?

**Bulgogi**는 **Boost.Beast** 위에 얇게 덧씌운 경량 wrapper 입니다.  
Boost.Beast의 low-level API 를 가리지 않고,  
반복적인 작업을 단순화하는 데에만 초점을 둡니다.  
필요하면 언제든지 Beast의 원래 개면(Interface)에 직접 접근할 수 있습니다.

> "야생의 Beast와 전투하지 말고, 불고기처럼 잘 료리된 걸 먹자."

즉, 불고기처럼 간단하고 편리하게 사용할 수 있도록
다음과 같은 기능을 제공합니다:

* 명시적으로 Beast session 과 상태를 관리할 필요 없음
* 복잡한 Asio socket 처리 필요 없음
* template metaprogramming 또는 build-time trick 없음

Bulgogi 는 Beast 를 **간편하게 가공한 판본**입니다.  
필요시 low-level 접근도 가능하지만, **웬만한 경우에는 이 "불고기 판본"으로 충분합니다.**

---

## 📄 License

MIT License © 2025 [bulgogi-framework](https://github.com/bulgogi-framework)
본 항목는 자유롭게 사용 및 기여 가능하며,
[JeongHan-Bae](https://github.com/JeongHan-Bae) 가 유지·관리합니다.

---

## 🤝 참고 및 기여자

* [Boost.Beast](https://github.com/boostorg/beast) — "HTTP Engine"의 핵심
* [jh-toolkit](https://github.com/JeongHan-Bae/jh-toolkit) — Type 안정성과 POD 도구들(Utilities) 제공

"Third Party License" 관련 상세 내용은 [NOTICE.md](./NOTICE.md) 를 참고하세요.
