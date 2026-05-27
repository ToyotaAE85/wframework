#ifndef WEIRD_FRAMEWORK_HPP
#define WEIRD_FRAMEWORK_HPP

#include <vector>
#include <string>
#include <stdexcept>

// C++ 20
#include <source_location>
#include <format>


#define CNCT(a, b) a##b
#define CONCAT(a, b) CNCT(a, b)


using WFuncPtr = void(*)();

struct TestCase {
	std::string name_;
	WFuncPtr func_;
};


inline std::vector<TestCase>& registry() {
	static std::vector<TestCase> reg_{};
	return reg_;
}


inline std::string retrieve_msg_impl(
	const std::string& msg, 
	const std::source_location& loc = std::source_location::current()
) {
	return std::format("{} {} ({}|{})",
		msg,
		loc.file_name(),
		loc.line(),
		loc.column()
	);
}


inline void requires_impl(bool bcond, const std::string& scond) {
	if (!bcond)
		throw std::runtime_error(
			retrieve_msg_impl(std::format("Required Condition failed: [{}]", scond))
		);
}


// kept for convenience

template <typename A, typename B>
void check_impl(
	bool eq, const A& lhs, const B& rhs,
	const std::string& sl, const std::string& sr
) {
	if (eq && !(lhs == rhs))
		throw std::runtime_error(
			retrieve_msg_impl(std::format("Equality check failed: [{} != {}]", sl, sr))
		);
	else if (!eq && (lhs == rhs))
		throw std::runtime_error(
			retrieve_msg_impl(std::format("Inequality check failed: [{} == {}]", sl, sr))
		);
}


#define WTESTCASE(name)											\
	void CONCAT(WF, __LINE__)();								\
	static auto CONCAT(WL, __LINE__) = [](){					\
		registry().push_back({name, CONCAT(WF, __LINE__)});		\
		return true;											\
	}();														\
	void CONCAT(WF, __LINE__)()	


#define WREQUIRES(cond)							\
	requires_impl(cond, #cond);


#define WCHECK_EQ(lhs, rhs)						\
	check_impl(true, lhs, rhs, #lhs, #rhs);


#define WCHECK_NEQ(lhs, rhs)					\
	check_impl(false, lhs, rhs, #lhs, #rhs);


#define WCHECK_OPERATOR(lhs, op, rhs)							\
	do {														\
		const auto lhs_ { (lhs) };								\
		const auto rhs_ { (rhs) };								\
		if (!(lhs_ op rhs_))									\
			throw std::runtime_error(							\
				retrieve_msg_impl(								\
					std::format(								\
						"Operator check failed: [{} {} {}]",	\
							#lhs, #op, #rhs))					\
			);													\
	} while (0);

void run_tests();

#endif 