#include <atomic>
#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>


std::atomic<bool> x,y;
std::atomic<int> z;

void write_x()
{
	x.store(true,std::memory_order_release);
}


void write_y()
{
	y.store(true,std::memory_order_release);
}


void read_x_then_y()
{
	while(!x.load(std::memory_order_acquire));
	if(y.load(std::memory_order_acquire))
		++z;
}


void read_y_then_x()
{
	while(!y.load(std::memory_order_acquire));
	if(x.load(std::memory_order_acquire))
		++z;
}


int do_main()
{
x=false;
y=false;
z=0;
std::thread a(write_x);
std::thread b(write_y);
std::thread c(read_x_then_y);
std::thread d(read_y_then_x);
a.join();
b.join();
c.join();
d.join();
assert(z.load()!=0);
return z;
}


int main()
{
	for (int i{0u}; i < 1'000'000; ++i)
		do_main();
	std::cout << "Bye bye Paul R\n";
}



#ifdef PAUL
std::atomic<bool> x, y, ready;
std::atomic<int> z;


void set_x_to_true()
{
	//std::this_thread::sleep_for( std::chrono::milliseconds(100) );
	while(!ready) ;
	x.store( true, std::memory_order_release );
}


void set_y_to_true()
{
	while(!ready) ;
	y.store(true, std::memory_order_release);
	//std::cout << "lINE " << __LINE__ << std::endl;
}


void read_x_then_y()
{
	//std::cout << "lINE " << __LINE__ << std::endl;
	while(!ready) ;
	while(!x.load(std::memory_order_acquire));
	if(y.load(std::memory_order_acquire))
		++z;
}


void read_y_then_x()
{
	//std::cout << "lINE " << __LINE__ << std::endl;
	while(!ready) ;
	while(!y.load(std::memory_order_acquire));
	if(x.load(std::memory_order_acquire))
		++z;
}


/*void read_x()
{
	//std::this_thread::sleep_for( std::chrono::milliseconds(100) );
	x.load( std::memory_order_release );

	std::cout << "X is " << std::boolalpha << x.load() << std::endl;
}

void do_main2()
{
	x = false;
	//x=false; y=false; z=0;
	std::thread a(set_x_true);
	std::thread b(read_x);

	a.join(); b.join();
}*/



int do_main()
{
	x=false; y=false; z=0; ready.store(false);
	std::thread a(set_x_to_true);
	std::thread b(set_y_to_true);
	std::thread c(read_x_then_y);
	std::thread d(read_y_then_x);

	std::this_thread::sleep_for( std::chrono::microseconds(100) );
	ready.store(true);
	a.join(); b.join(); c.join(); d.join();

	assert(z.load() >= 1);
	return z;
}



int main()
{
	for (int i = 0; i < 10000; ++i)
		do_main();
	std::cout << "Finished\n";
}


/*
int main()
{
	int array[3] = {};
	for (int i = 0; i < 10; ++i)
	{
		auto result = do_main();
		if (result == 1) std::cout << "Previous 4 statements\n";
		++array[result];
	}
	std::cout << "array[0] = " << array[0] << "\n";
	std::cout << "array[1] = " << array[1] << "\n";
	std::cout << "array[2] = " << array[2] << "\n";
	return 0;
}
*/
#endif
