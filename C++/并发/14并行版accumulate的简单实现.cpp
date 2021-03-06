#include<iostream>
#include<thread>
#include<numeric>//提供std::accumulate
#include<vector>

template<typename Iterator,typename T>
struct accumulate_block
{
	void operator()(Iterator first, Iterator last, T& result)
	{
		result = std::accumulate(first, last, result);
	}
};
template<typename Iterator,typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
	unsigned long const length = std::distance(first, last);//返回从 first 到 last 的路程。
	if (!length)
		return init;
	unsigned long const min_per_thread = 25;
	unsigned long const max_threads = (length + min_per_thread - 1) / min_per_thread;//如果是十个元素，那么结果为1
	unsigned long const hardware_threads = std::thread::hardware_concurrency();//返回值支持的并发线程数。若该值非良定义或不可计算，则返回 ​0，我电脑为16
	unsigned long const num_threads = std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);//16返回16,min(16,1)，返回1，num_threads为1
	unsigned long const block_size = length / num_threads;//10/1，那么size为10
	std::vector<T> results(num_threads);//1
	std::vector<std::thread>threads(num_threads - 1);//以thread对象为元素，初始化为1-1,0
	Iterator block_start = first;//一开始，开始迭代器位置为开始first
	for (unsigned long i = 0; i < (num_threads - 1); ++i)//1-1,0，10个元素会直接跳过
	{
		Iterator block_end = block_start;//结束迭代器
		std::advance(block_end, block_size);//advance第一个参数迭代器，第二个参数移动的元素个数，在这里的作用是划分，让结束迭代器移动，这样开始和结束中间的元素就是线程处理的元素
		threads[i] = std::thread(accumulate_block<Iterator, T>(),block_start, block_end, std::ref(results[i]));//一个函数对象，两个参数，构造了匿名thread对象开启线程
		block_start = block_end;//增加，当上一个线程开启完毕后让开始迭代器赋值为上一个的末尾迭代器
	}
	accumulate_block<Iterator, T>() ( block_start, last, results[num_threads - 1] );//10个元素则在这里就计算完，55，如果数据很大那么最后一块数据会在这里求和也就是主线程要在这里计算
	for (auto& entry : threads)
		entry.join();

	return std::accumulate(results.begin(), results.end(), init);//accumulate求和算法，init为起始值，在多线程求出每一堆元素的和后放入result，这一步操作是让他们的值加起来
}

int main() {
	int n[33]{ 10,9,8,7,6,5,4,3,2,1,1,10,25,65,98,32,31,14,15,15,18,18,9,5,9,8,8,8,8,9,6,6,55 };
	std::cout << parallel_accumulate(n, n + 33, 0) << std::endl;
}
//仔细看来也很简单，关注线程和result，以及开始与结束迭代器的移动。
//31，32,33有详细的介绍