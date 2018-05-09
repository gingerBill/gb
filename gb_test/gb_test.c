#define GB_IMPLEMENTATION
#include "gb.h"
#include <stdio.h>
#include<math.h> // pow
#include <float.h> // DBL_MIN, DBL_MAX
#include "gb_test.h"

void *AllocatorProc(void *allocator_data, gbAllocationType type, \
		   isize size, isize alignment,                 \
		   void *old_memory, isize old_size,            \
		   u64 flags)
{
	switch (type)
	{
	case gbAllocation_Alloc:
	{
		int nMemorySize = size;
		if(alignment>4)
			nMemorySize = (size / alignment)*alignment + alignment;
		allocator_data = malloc(nMemorySize);
	}
	break;
	case gbAllocation_Free:
	{
		free(allocator_data);
		allocator_data = NULL;
		free(old_memory);
		old_memory = NULL;
	}
	break;
	case gbAllocation_FreeAll:
	{
		free(allocator_data);
		allocator_data = NULL;
		free(old_memory); 
		old_memory = NULL;
	}
	break;
	case gbAllocation_Resize:
	{
#ifdef _DEBUG
		printf("size: %d  oldSize: %d \n", size, old_size);
#endif //_DEBUG
		int nMemorySize = (size / alignment)*alignment + alignment;
		void *pData = malloc(nMemorySize);
		memcpy(pData, old_memory, nMemorySize);
		free(old_memory);
		old_memory = NULL;
		allocator_data = pData;
	}
	break;
	default:
		break;
	}
	return allocator_data;;
}

void gbStringDemo()
{
	//gbAllocator allocator = { 0 };
	//allocator.proc = &AllocatorProc;
	gbAllocator allocator = gb_heap_allocator();
	gbString str = gb_string_make(allocator, "Hello");

	gbString other_str = gb_string_make_length(allocator, ", ", 2);

	str = gb_string_append(str, other_str);
	str = gb_string_appendc(str, "world!");

	gb_printf("%s\n", str); // Hello, world!

	gb_printf("str length = %d\n", gb_string_length(str));

	str = gb_string_set(str, "Potato soup");
	gb_printf("%s\n", str); // Potato soup

	str = gb_string_set(str, "Hello");
	other_str = gb_string_set(other_str, "Pizza");
	if (gb_string_are_equal(str, other_str))
		gb_printf("Not called\n");
	else
		gb_printf("Called\n");

	str = gb_string_set(str, "Ab.;!...AHello World       ??");
	str = gb_string_trim(str, "Ab.;!. ?");
	gb_printf("%s\n", str); // "Hello World"

	gb_string_free(str);
	gb_string_free(other_str);
}


isize ThreadFun(struct gbThread *thread)
{
	do
	{
		printf("%s\n",(char*)thread->user_data);
		Sleep(300);
	} while (!thread->user_index);
	printf("线程退出了\n");
	return 0;
}

void gbThreadDemo()
{
	// 创建线程结构体
	gbThread  thread1;
	gb_thread_init(&thread1);
	gb_thread_set_name(&thread1, "Thread1");
	// 启动线程
	gb_thread_start(&thread1, ThreadFun, "Hello,北京");
	// 等待线程运行完毕
	//gb_thread_join(&thread1);

	bool bRet = gb_thread_is_running(&thread1);
	if (bRet)
		printf("Running\n");

	Sleep(1000);
	// 找个变量作为线程退出的标志
	thread1.user_index = 1;
	// 销毁其它线程结构体
	gb_thread_destroy(&thread1);
}

void MemOperaTest()
{
	char * buf = (char*)gb_malloc(128);
	gb_zero_size(buf, 128);
	gb_memcopy(buf, "Hello,北京！", strlen("Hello,北京！"));
	// 0 表示相等
	int nRet = gb_memcompare(buf, "Hello,北京！", strlen("Hello,北京！"));
	char* memCh = gb_memchr(buf, "北", strlen("Hello,北京！"));
	char* memRCh = gb_memrchr(buf, 0x6c6c, strlen("Hello,北京！"));
	gb_mfree(buf);
}

void gbAtomicDemo()
{
	gbAtomic32 atominc32;
	atominc32.value = 0xf0f0;
	gb_atomic32_fetch_add(&atominc32, 0x3);
	int nValue = gb_atomic32_load(&atominc32);
	gb_atomic32_fetch_and(&atominc32, 0x0f0f);
	gb_atomic32_try_acquire_lock(&atominc32);
	// spinlock是专为防止多处理器并发而引入的一种锁。
	gb_atomic32_spin_lock(&atominc32, 3);

	gbAtomic64 atominc64;
	atominc64.value = 64;
	gb_atomic64_try_acquire_lock(&atominc64);
}

isize ThreadWaiteSem(struct gbThread *thread)
{
	printf("步入进程\n");
	gb_semaphore_wait(thread->user_data);
	printf("退出进程\n");
	return 0;
}

void gbsemaphoreDemo()
{
	gbSemaphore sem;
	gb_semaphore_init(&sem);
	gbThread thread1;
	gb_thread_init(&thread1);
	gb_thread_start(&thread1, ThreadWaiteSem, &sem);
	Sleep(200);
	gb_semaphore_post(&sem, 1);
	Sleep(200);
	gb_semaphore_release(&sem);
	gb_semaphore_destroy(&sem);
}

int g_nNum = 100;
isize ThreadAddNum(gbThread *thread)
{
	// 总共加了 5000
	for(int i=0;i<10000;i++)
	{	
		gb_mutex_lock(thread->user_data);
		g_nNum += 5;
		gb_mutex_unlock(thread->user_data);
	} 
	return 0;
}
isize ThreadSebNum(gbThread *thread)
{
	// 总共减了 1000
	for (int i = 0; i<10000; i++)
	{
		gb_mutex_lock(thread->user_data);
		g_nNum -= 1;;
		gb_mutex_unlock(thread->user_data);
	} 
	return 0;
}

void gbMutexTest()
{
	gbMutex m;
	gb_mutex_init(&m);
	/*{
	gb_mutex_lock(&m);
	gb_mutex_unlock(&m);
	}*/
	gbThread thread1, thread2;
	gb_thread_init(&thread1);
	gb_thread_init(&thread2);
	g_nNum = 100;
	gb_thread_start(&thread1, ThreadAddNum, &m);
	gb_thread_start(&thread2, ThreadSebNum, &m);

	// 等待线程结束
	gb_thread_join(&thread1);
	gb_thread_join(&thread2);
	if (g_nNum == 40100)
	{
		printf("data is Ok\n");
	}
	else
	{
		printf("data is uncorrect!\n");
	}
	gb_thread_destroy(&thread1);
	gb_thread_destroy(&thread2);
	gb_mutex_destroy(&m);
}

void gbPoolDemo()
{
	gbPool gbpool;
	int nBlockCount = 10000;
	int nBlockSize = 100;
	gb_pool_init(&gbpool, gb_heap_allocator(), nBlockCount, nBlockSize);
	gbAllocator gbPoolAllocator = gb_pool_allocator(&gbpool);

	// 内存块的分配大小，每次只能非配内存块的大小
	// 需要的内存大于内存块的多分配几次
	char* pData1 = gb_alloc(gbPoolAllocator, nBlockSize);
	memset(pData1, 0x11, nBlockSize);


	char* pData2 = gb_alloc(gbPoolAllocator, nBlockSize);
	memset(pData2, 0x22, nBlockSize);

	gb_pool_free(&gbpool);
}

void gbFreeListDemo()
{
	gbFreeList gbfreeList;
	int nMemSize = 0xff00;
	gb_free_list_init_from_allocator(&gbfreeList, gb_heap_allocator(), nMemSize);
	// 获取FreeList专用的内存分配
	gbAllocator gbListAllocator = gb_free_list_allocator(&gbfreeList);

	gbString str1 = gb_string_make(gbListAllocator, "Hello,北京！1");
	gbAllocationHeader *header1 = gb_allocation_header(str1);

	gbString str2 = gb_string_make(gbListAllocator, "Hello,北京！22");
	gbAllocationHeader *header2 = gb_allocation_header(str2);

	gbString str3 = gb_string_make(gbListAllocator, "Hello,北京！333");
	gbAllocationHeader *header3 = gb_allocation_header(str3);
}

void gbVirtualMemoryDemo()
{
	int nAlignSize = 0;
	gb_virtual_memory_page_size(&nAlignSize);
	int VmSize = nAlignSize * 2;

	// 申请一块虚拟内存，第一个参数为NULL
	gbVirtualMemory gbVirtualMem = gb_vm_alloc(NULL, VmSize);
	gb_memset(gbVirtualMem.data, 0x11, 20);
	// 设置为读写
	gb_vm_purge(gbVirtualMem);

	// 释放掉虚拟内存，重新生成一个虚拟内存
	// 参数二好像要对其，否则没啥用
	// 参数三新的虚拟内存大小
	gbVirtualMemory vbVmTrim = gb_vm_trim(gbVirtualMem, 10, 100);
	gb_memset(vbVmTrim.data, 0x22, vbVmTrim.size);

	gb_vm_free(gbVirtualMem);
}

void dgArenaMemAlloc()
{
	// 先申请一块内存作为父亲
	gbAllocator ArenaAlloc = gb_heap_allocator();
	gbArena parent_arena = { 0 };
	gb_arena_init_from_allocator(&parent_arena, ArenaAlloc, 100);

	// 从已经分配的内存中使用指定的内存，大小必须4字节对齐
	gbArena usingMem1 = { 0 };
	gb_arena_init_sub(&usingMem1, &parent_arena, 8 * 5);
	// 使用内存
	if (usingMem1.physical_start)
	{
		// ？？？？　用来做引用计数的
		gbTempArenaMemory  gbTmpArenaMemory1 = gb_temp_arena_memory_begin(&usingMem1);
		memset(usingMem1.physical_start, 0x11, usingMem1.total_size);
		gb_temp_arena_memory_end(gbTmpArenaMemory1);
	}

	gbArena usingMem2 = { 0 };
	gb_arena_init_sub(&usingMem2, &parent_arena, 8 * 5);
	if (usingMem2.physical_start)
	{
		memset(usingMem2.physical_start, 0x22, usingMem2.total_size);
	}

	gbArena usingMem3 = { 0 };
	gb_arena_init_sub(&usingMem3, &parent_arena, 8 * 5);
	if (usingMem3.physical_start)
		memset(usingMem3.physical_start, 0x33, usingMem3.total_size);

	// 设置自定义内存的对齐大小，必须是2的幂次方
	int alignment1 = pow(2, 4); // 2的幂次方
	int alignment_offset = gb_arena_alignment_of(&parent_arena, alignment1);
	// 获取剩余内存大小，需要内存对齐的大小
	int alignment2 = pow(2, 3); //2的幂次方
	int nSizeRemain = gb_arena_size_remaining(&parent_arena, alignment2);

	gb_arena_free(&parent_arena);
}

void gbScratchMemoryTest()
{
	// 用来判断已经非配的内存是否已经使用（指针地址比较）
	gbScratchMemory gbScarathcMem;
	gbAllocator Allocator = gb_scratch_allocator(&gbScarathcMem);
	char* pData = gb_malloc(1000);
	gb_scratch_memory_init(&gbScarathcMem, pData, 1000);
	gb_scratch_memory_is_in_use(&gbScarathcMem, pData);
	gb_mfree(pData);
}

// 返回0表示相等
// UnDone
int MyCmpFun(void const *a, void const *b)
{
	return -1;
}

void gbArrayDemo()
{
	isize i;
	int test_values[] = { 4, 2, 1, 7 };
	gbAllocator a = gb_heap_allocator();
	gbArray(int) items;

	gb_array_init(items, a);

	gb_array_append(items, 1);
	gb_array_append(items, 4);
	gb_array_append(items, 9);
	gb_array_append(items, 16);

	gbArrayHeader *gb__ah = GB_ARRAY_HEADER(items);

	items[1] = 3; // Manually set value
				  // NOTE: No array bounds checking

	for (i = 0; i < gb_array_count(items); i++)
		gb_printf("%d\n", items[i]);
	// 1
	// 3
	// 9
	// 16

	gb_array_clear(items);

	gb_array_appendv(items, test_values, gb_count_of(test_values));
	for (i = 0; i < gb_array_count(items); i++)
		gb_printf("%d\n", items[i]);
	// 4
	// 2
	// 1
	// 7

	gb_array_free(items);
}

void gbHashTest()
{
	char* pStr = "Hello,北京！";
	u32 u32Value = gb_adler32(pStr, gb_strlen(pStr));
	u32 uCrc32 = gb_crc32(pStr, gb_strlen(pStr));
	u64 uCrc64 = gb_crc64(pStr, gb_strlen(pStr));

	u32 fnv32 = gb_fnv32(pStr, gb_strlen(pStr));
	u64 fnv64 = gb_fnv64(pStr, gb_strlen(pStr));
	u32 fnv32a = gb_fnv32a(pStr, gb_strlen(pStr));
	u64 fnv64a = gb_fnv64a(pStr, gb_strlen(pStr));

	u32 murmur32 = gb_murmur32(pStr, gb_strlen(pStr));
	u64 murmur64 = gb_murmur64(pStr, gb_strlen(pStr));

	u32 murmur32_seed = gb_murmur32_seed(pStr, gb_strlen(pStr), 0xf0e0d0c0);
	u64 murmur64_seed = gb_murmur64_seed(pStr, gb_strlen(pStr), 0x0102030405060708);
}

int gbFileDemo()
{
	gbFile f;
	gbFileError error = gb_file_open_mode(&f, gbFileMode_Read, "D:\\picture.bmp");
	if (error != gbFileError_None)
		return -1;
	{
		i64 nFileSize = gb_file_size(&f);
		char* pFileData = gb_malloc(nFileSize);
		b32 nRead = gb_file_read(&f, pFileData, nFileSize);
		gbFile fSave;
		gb_file_close(&f);
		if (gb_file_exists("D:\\Picture_copy.bmp"))
			gb_file_remove("D:\\Picture_copy.bmp");
		error = gb_file_create(&fSave, "D:\\Picture_copy.bmp");
		if (error != gbFileError_None)
		{
			gb_mfree(pFileData);
			return -1;
		}
		gb_file_write(&fSave, pFileData, nFileSize);
		gb_file_close(&fSave);
		gb_mfree(pFileData);
	}
	return 0;
}

void dbPathTest()
{
	char* path = "D:\\sample.jpg";
	gb_path_is_absolute(path);
	b32 b32Ret1 = gb_path_is_relative(path);
	b32 b32Ret2 = gb_path_is_root(path);
	char* outPaht1 = gb_path_base_name(path);
	char* outPath2 = gb_path_extension(path);
	char* outPath3 = gb_path_get_full_name(gb_heap_allocator(), path);
	gb_mfree(outPath3);
}

void gbDllHandleTest()
{
	gbDllHandle DLLHandle = gb_dll_load("user32.dll");
	if (DLLHandle)
	{
		gbDllProc  dllProc = gb_dll_proc_address(DLLHandle, "MessageBoxA");
		if (dllProc)
		{
			typedef int (WINAPI*Msg)(HWND hWnd, char*, char*, int);
			Msg msg = (Msg)dllProc;
			msg(NULL, "Msg", "Tip", 0);
		}
		gb_dll_unload(DLLHandle);
	}
}

void gbRandomTest()
{
	gbRandom ramdom;
	gb_random_init(&ramdom);
	u32 gen_u32 = gb_random_gen_u32(&ramdom);
	u32 u32_unique = gb_random_gen_u32_unique(&ramdom);
	u64 gen_u64 = gb_random_gen_u64(&ramdom); // NOTE(bill): (gb_random_gen_u32() << 32) | gb_random_gen_u32()
	isize gen_isize = gb_random_gen_isize(&ramdom);
	i64 range_i64 = gb_random_range_i64(&ramdom, 0x1020304050607080, 0xf0f0f0f010203040);
	isize range_isize = gb_random_range_isize(&ramdom, 0x10203040, 0xf0f0f0f0);

	f64 range_f64 = gb_random_range_f64(&ramdom, DBL_MIN, DBL_MAX);
}

void gbPrintfDemo()
{
	gb_printf_err("%s %d\n", "Hello,北京！", 123);

	gbFile f;
	gbFileError error = gb_file_create(&f, "D:\\log.txt");
	if (error == gbFileError_None)
	{
		gb_fprintf(&f, "%s %d\n", "Hello,北京！", 123);
		gb_file_close(&f);
	}
}

int main(int argc, char **argv)
{
	// Uncomment this to run function test
	 gbStringDemo();
	 gbThreadDemo();
	 MemOperaTest();	
	 gbAtomicDemo();
	 gbsemaphoreDemo();
	 gbMutexTest();
	 gbPoolDemo();
	 gbFreeListDemo();
	 gbVirtualMemoryDemo();
	 dgArenaMemAlloc();
	 gbScratchMemoryTest();
	 gbArrayDemo();
	 gbHashTest();
	 gbFileDemo();
	 dbPathTest();
	 gbDllHandleTest();
	 gbRandomTest();
	 gbPrintfDemo();


	//gbSync gbsync;
	//gb_sync_init(&gbsync);

	//gbAffinity gbaffinity;
	//gb_affinity_init(&gbaffinity);
	//// 这个数值不能乱填
	//gb_affinity_set(&gbaffinity, gbaffinity.core_count-1,1);

	//gb_affinity_thread_count_for_core(&gbaffinity, gbaffinity.core_count-1);
	//gb_affinity_destroy(&gbaffinity);


#if defined(GB_SYSTEM_WINDOWS)
	// No Support Chinese
	char* pStr1 = "Hello";
	gbAllocator allocator = gb_heap_allocator();
	int w_len_ = 0;
	wchar_t* wStr = gb__alloc_utf8_to_ucs2(allocator, pStr1,&w_len_);
#endif // #if defined(GB_SYSTEM_WINDOWS)

	u32 u32NoiseTime = gb__get_noise_from_time();
	return 0;
}