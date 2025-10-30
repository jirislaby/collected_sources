#include <iostream>

#include <nlohmann/json.hpp>

using Json = nlohmann::ordered_json;

int main()
{
	Json j(Json::value_t::array);
	const std::string str;
	try {
		auto o = Json::object({ "p", "str" });
		std::cout << o.type_name() << '\n';
	} catch (const Json::exception &e) {
		std::cerr << e.what() << '\n';
	};
	Json obj(Json::value_t::object);
	obj.push_back({ "cve2", "bla" });
	obj.push_back({ "sha", "bla" });
	obj.push_back({ "subsys", "val3" });
	obj.push_back({ "emails", nullptr });
	obj.back().emplace_back("a@b.cz");
	obj.back().emplace_back("sss");
	j.push_back(std::move(obj));
	std::cout << std::setw(2) << j << '\n';
	return 0;
}

