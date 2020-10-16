function
emit_thin_space(u)
{
	u.a.push({type:SPACE, height:0, depth:0, width:THIN_SPACE_WIDTH});
}
