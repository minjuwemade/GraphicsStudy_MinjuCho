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

## Command Queue

- 의미: CPU → GPU 작업 전달 매개체
- 역할:
    - 병렬 처리: CPU는 Command List를 만들어 큐에 쌓고, GPU는 큐에서 명령을 꺼내 실행하여 CPU와 GPU가 독립적으로 작업하게 된다.
- 사용:
    - Command Queue 생성
        
        ```cpp
        virtual HRESULT __stdcall ID3D12Device::CreateCommandQueue(const D3D12_COMMAND_QUEUE_DESC *pDesc, const IID &riid, void     **ppCommandQueue)
        ```
        
        - `D3D12_COMMAND_QUEUE_DESC` 구조체를 채운 후 위의 함수를 호출.
    - 명령 리스트가 사용할, 메모리 생성
        
        ```cpp
        virtual HRESULT __stdcall ID3D12Device::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE type, const IID &riid, void **    ppCommandAllocator)
        ```
        
        - 여러 명령 리스트가 하나의 Allocator 재활용 가능
            
            (하지만 같은 할당자로 두 명령 리스트를 연달아 생성하면 오류 발생.
            아래의 `ID3D12GraphicsCommandList::Close()`와 `ID3D12GraphicsCommandList::Reset()` 를 모두 호출한 후 사용 가능)
            
    - Allocator Reset
        
        ```cpp
        virtual HRESULT __stdcall ID3D12GraphicsCommandList::Reset(ID3D12CommandAllocator *pAllocator, ID3D12PipelineState *    pInitialState)
        ```
        
    - 명령 리스트 생성
        
        ```cpp
        virtual HRESULT __stdcall ID3D12Device::CreateCommandList(UINT nodeMask, D3D12_COMMAND_LIST_TYPE type,     ID3D12CommandAllocator *pCommandAllocator, ID3D12PipelineState *pInitialState, const IID &riid, void **    ppCommandList)
        ```
        
        - 인자로 명령 리스트가 사용할 메모리, 즉 위에서 `CreateCommandAllocator` 로 생성한 할당자를 넣는다.
    - Command List 다 채운 후 꼭 닫기.
        
        ```cpp
        virtual HRESULT __stdcall ID3D12GraphicsCommandList::Close()
        ```
        
    - Command Queue를 통해 해당 Command List 실행
        
        ```cpp
        virtual void __stdcall ID3D12CommandQueue::ExecuteCommandLists(UINT NumCommandLists, ID3D12CommandList *const *    ppCommandLists)
        ```
        
        - Allocator 와 함께 생성된 Command List가 Close() 가 되어있어야만 실행 가능.