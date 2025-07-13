#include "DXContext.h"
#include <cstdlib>

/*
*   GPU
*   <-->
*   CommandQueue (WorkSet, WorkSet, Sync)
*    <--
*   CPU WorkSet(Setup, Setup, ChangeState, Setup, Draw) == CommandList
*/


bool DXContext::Init()
{
	if (FAILED(D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device))))
	{
		return false;
	}

	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	cmdQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cmdQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	cmdQueueDesc.NodeMask = 0;
	cmdQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	if (FAILED(m_device->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&m_cmdQueue))))
	{
		return false;
	}

	if (FAILED(m_device->CreateFence(m_fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence))))
	{
		return false;
	}

	// Can share any event between processes, but do not share in this context
	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (m_fenceEvent == nullptr)
	{
		return false;
	}

	if (FAILED(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_cmdAllocator))))
	{
		return false;
	}

	if (FAILED(m_device->CreateCommandList1(0, D3D12_COMMAND_LIST_TYPE_DIRECT, D3D12_COMMAND_LIST_FLAG_NONE, IID_PPV_ARGS(&m_cmdList))))
	{
		return false;
	}
	return true;
}

void DXContext::Shutdown()
{
	m_cmdList.Release();
	m_cmdAllocator.Release();
	if (m_fenceEvent)
	{
		CloseHandle(m_fenceEvent);
	}
	m_cmdQueue.Release();
	m_device.Release();
}

void DXContext::SignalAndWait()
{
	// If this Command Queue has done, make m_fence's internal value as m_fenceValue
	// (Signal is also an another command)
	// m_fence: has GPU counter
	// m_fenceValue: goal counter made in CPU
	m_cmdQueue->Signal(m_fence, ++m_fenceValue);

	//// 1. Wait until GPU's Signal (This logic is not good.)
	//while (m_fence->GetCompletedValue() < m_fenceValue)
	//{
	//	// yield CPU to another thread
	//	SwitchToThread();
	//}

	// Ideal Logic: Sleep this thread until the work is done -> Wake up and put it in front of the queue
	// 2. Call event when received completion message from GPU
	// Background: GPU driver updates fence, DirectX signals the event
	if (SUCCEEDED(m_fence->SetEventOnCompletion(m_fenceValue, m_fenceEvent)))
	{
		if (WaitForSingleObject(m_fenceEvent, 20000) != WAIT_OBJECT_0)
		{
			// Failure if not completed in 20s
			std::exit(-1);
		}
	}
	else
	{
		std::exit(-1);
	}
}

ID3D12GraphicsCommandList* DXContext::InitCommandList()
{
	m_cmdAllocator->Reset();
	m_cmdList->Reset(m_cmdAllocator.Get(), nullptr);
	return m_cmdList;
}

void DXContext::ExecuteCommandList()
{
	if (SUCCEEDED(m_cmdList->Close()))
	{
		ID3D12CommandList* lists[] = { m_cmdList };
		m_cmdQueue->ExecuteCommandLists(1, lists);
		SignalAndWait();
	}
}
