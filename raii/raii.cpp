#include <iostream>
#include <memory>

struct Managed {
public:
	Managed() = delete;
	explicit Managed(unsigned val) : m_i(val) {
		std::cout << __PRETTY_FUNCTION__ << ": " << this << " val=" << i() << '\n';
	}
	~Managed() {
		std::cout << __PRETTY_FUNCTION__ << ": " << this << " val=" << i() << '\n';
	}

	unsigned i() const { return m_i; }
private:
	unsigned m_i;
};

class Wrapper {
public:
	static Wrapper getOne(unsigned i) {
		Wrapper w(new Managed(i));
		return w;
	}
	Wrapper(Wrapper &&other) noexcept : m_m(std::move(other.m_m)) {
		std::cout << __PRETTY_FUNCTION__ << ": " << &other << "->" << this <<
			     " managed=" << m() << " val=" << m()->i() << '\n';
	}
	Wrapper &operator=(Wrapper &&other) noexcept {
		if (this != &other) {
			m_m = std::move(other.m_m);
		}
		std::cout << __PRETTY_FUNCTION__ << ": " << &other << "->" << this <<
			     " managed=" << m() << " val=" << m()->i() << '\n';
		return *this;
	}
	~Wrapper() {
		std::cout << __PRETTY_FUNCTION__ << ": " << this << " managed=" << m_m.get() <<
			     " val=";
		if (m_m)
			std::cout << m()->i();
		else
			std::cout << "N/A";
		std::cout << '\n';
	}

	Managed *m() const { return m_m.get(); }

private:
	explicit Wrapper(Managed *ptr) : m_m(ptr) {
		std::cout << __PRETTY_FUNCTION__ << ": " << this << " managed=" << ptr <<
			     " val=" << m_m->i() << '\n';
	}

	std::unique_ptr<Managed> m_m;
};

static Wrapper fun(unsigned i)
{
	return Wrapper::getOne(i);
}

static void funMove(const Wrapper w)
{
	std::cout << __func__ << ": " << &w << " val=" << w.m()->i() << '\n';
}

static void funRef(const Wrapper &w)
{
	std::cout << __func__ << ": " << &w << " val=" << w.m()->i() << '\n';
}

int main()
{
	std::cout << __func__ << ": one" << '\n';
	auto one = Wrapper::getOne(1);
	std::cout << __func__ << ": two" << '\n';
	auto two = Wrapper::getOne(2);
	std::cout << __func__ << ": three" << '\n';
	auto three = fun(3);

	funRef(one);
	funRef(two);
	funRef(three);
	funMove(std::move(two));
	one = std::move(three);

	std::cout << __func__ << ": done" << '\n';

	return 0;
}

