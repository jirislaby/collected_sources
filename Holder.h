#pragma once

#include <utility>

template <typename T, typename Destroy>
class Holder {
public:
	Holder() : m_valid(false) {}

	explicit Holder(T x) noexcept : m_valid(true), m_x(std::move(x)) {}

	Holder(Holder &&o) noexcept : m_valid(o.m_valid), m_x(std::move(o.m_x)) {
		o.invalidate();
	}

	Holder &operator=(Holder &&o) noexcept {
		if (this != &o) {
			auto valid = o.m_valid;
			set(o.release(), valid);
		}
		return *this;
	}

	Holder(const Holder &) = delete;
	Holder &operator=(const Holder &) = delete;

	~Holder() noexcept { destroy(); }

	void set(T t, bool valid = true) {
		destroy();
		if (valid)
			m_x = std::move(t);
		m_valid = valid;
	}

	T release() {
		invalidate();
		return std::move(m_x);
	}

	bool valid() const { return m_valid; }
	operator bool() const { return valid(); }

	const T &get() const { return m_x; }
	explicit operator const T &() const { return get(); }
private:
	void destroy() {
		if (m_valid)
			Destroy{}(release());
	}

	void invalidate() {
		m_valid = false;
	}

	bool m_valid;
	T m_x;
};

