#include "../gb.hpp"

global gb::Heap_Allocator g_heap = {};

inline gb::Allocator*
default_allocator()
{
	return &g_heap;
}

int main(int argc, char** argv)
{
	// "Use" variables
	argc; argv;

	using namespace gb;

	{
		const u8 bytes[4] = {0x00, 0x20, 0xa7, 0x44};
		f32 a = reinterpret_cast<const f32&>(bytes);
		printf("%f\n", a);

		const f32 f = 1337.0f;
		u8* fb = (u8*)(&f);
		printf("0x%x%x%x%x\n", fb[0], fb[1], fb[2], fb[3]);

	}

	{
		auto table = hash_table::make<f32>(default_allocator());
		hash_table::set(&table, 123, 321.0f);
		hash_table::set(&table, 456, 654.0f);

		#define PGET(key, d) printf("%7d : %7f \n", key, hash_table::get(table, (key), (f32)(d)))

		PGET(123, 0);
		PGET(456, 0);
		PGET(789, 0);

		#undef PGET

	}

	{
		String hello = string::make(default_allocator(), "Hello");
		String world = string::make(default_allocator(), ", world!", 8);
		defer(string::free(hello));
		defer(string::free(world));

		string::append(&hello, world);
		printf("%s\n", hello);
	}

	for (u32 i = 0; i < 8; i++)
	{
		u64 bins[10] = {};
		auto gen_rand = random::make_random_device();
		auto gen = random::make_mt19937_64(random::next(&gen_rand));

		for (usize i = 0; i < 200000; i++)
		{
			u64 result = random::uniform_u64_distribution(&gen, 0, 9);
			bins[result]++;
		}

		for (usize i = 0; i < 10; i++)
		{
			printf("%2d : ", i);;
			u32 a = (u32)(bins[i] / 1000);
			for (u32 i = 0; i < a; i++)
				printf("*");
			printf("\n");
		}

		time::sleep(time::seconds(1));
	}

	while (getchar() != '\n')
		;


	return 0;
}
