## DebugLayer
- 의미: 디버깅 도우미

- 역할:

    - 메모리 누수 탐지:
        객체가 제대로 해제되지 않았는지 검사하여 메모리 누수를 찾아냄.

    - 디버그 정보 제공: 
        DirectX에서 디버그 정보를 출력해 개발 중 문제를 추적할 수 있도록 도와줌.

    - 주요 함수:


        ```virtual HRESULT __stdcall IDXGIDebug::ReportLiveObjects(GUID apiid, DXGI_DEBUG_RLO_FLAGS flags);```

- 참고 사항:

    - D3D12GetDebugInterface()나 ID3D12Debug::EnableDebugLayer() 함수는 릴리스 빌드에서도 컴파일 및 실행 가능.

    - 하지만 #ifdef _DEBUG 조건부 컴파일을 사용하는 이유는, 릴리스 빌드에서 디버그 관련 코드가 포함되지 않도록 하여 불필요한 오버헤드를 방지하기 위함.
