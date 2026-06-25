#include <iostream>
#include <string>

template<typename... More>
struct Packed {
	std::string str;
	using Tuple = std::tuple<More...>;
	Tuple members;
	void reset() {
		str = {};
		std::apply([](auto &... args) { ((args = {}), ...); }, members);
	}
	template<size_t idx>
	std::tuple_element_t<idx, Tuple> &get() {
		return std::get<idx>(members);
	}
};

template<typename... More>
std::ostream &operator<<(std::ostream &out, const Packed<More...> &p) {
	out << "[ '" << p.str << "'";
	std::apply([&out](const auto &... args) { ((out << ", " << args), ...); }, p.members);
	return out << " ]\n";
}

int main()
{
	{
		Packed p;
		p.str = "ahoj";
		std::cout << p;
	}
	{
		Packed<int> p;
		p.str = "ahoj2";
		p.get<0>() = 1;
		std::cout << p;
		p.reset();
		std::cout << p;
	}
	{
		Packed<int, int> p;
		p.str = "ahoj3";
		p.get<0>() = 2;
		p.get<1>() = 3;
		std::cout << p;
		p.reset();
		std::cout << p;
	}

	return 0;
}

