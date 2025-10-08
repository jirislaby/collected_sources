#include <chrono>
#include <iostream>
#include <string>
#include <vector>

int main()
{
	static constexpr const auto loops = 10'000'000U;
	static constexpr const auto dump = 3U;
	std::vector<std::string> a, b, c;

	const auto now1a = std::chrono::steady_clock::now();
	for (auto i = 0U; i < loops; ++i) {
		auto s = std::to_string(i) + " 123456789012345"; // more than 15 for SSO
		a.push_back(std::move(s));
	}
	const auto now1b = std::chrono::steady_clock::now();

	std::cout << "capa=" << a[0].capacity() << " size=" << a[0].size() << '\n';

	for (auto i = 0U; i < dump; ++i)
		std::cout << "after init: " <<  i << "=" << a[i] << '\n';

	const auto now2a = std::chrono::steady_clock::now();
	for (auto i = 0U; i < loops; ++i)
		b.push_back(a[i]);
	const auto now2b = std::chrono::steady_clock::now();

	for (auto i = 0U; i < dump; ++i)
		std::cout << "after copy: a[" << i << "]=" << a[i] << '\n';
	for (auto i = 0U; i < dump; ++i)
		std::cout << "after copy: b[" << i << "]=" << b[i] << '\n';

	const auto now3a = std::chrono::steady_clock::now();
	for (auto i = 0U; i < loops; ++i)
		b.push_back(std::move(a[i]));
	const auto now3b = std::chrono::steady_clock::now();

	for (auto i = 0U; i < dump; ++i)
		std::cout << "after move: a[" << i << "]=" << a[i] << '\n';
	for (auto i = 0U; i < dump; ++i)
		std::cout << "after move: b[" << i << "]=" << b[i] << '\n';

	using Dur = std::chrono::duration<double, std::chrono::milliseconds::period>;

	std::cout << "init time=" << std::chrono::duration_cast<Dur>(now1b - now1a).count() << " ms\n";
	std::cout << "copy time=" << std::chrono::duration_cast<Dur>(now2b - now2a).count() << " ms\n";
	std::cout << "move time=" << std::chrono::duration_cast<Dur>(now3b - now3a).count() << " ms\n";

	return 0;
}

