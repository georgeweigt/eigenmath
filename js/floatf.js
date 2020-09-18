function
floatf()
{
	float_subst();
	evalf();
	if (findf(stack[stack.length - 1], symbol(PI))) {
		float_subst();
		evalf();
	}
}
