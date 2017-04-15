START	{
    current = 0
}
{
    if (count == 0)
    {
	lower = $1
	uppernext = 1
    }
    else if (uppernext == 1)
    {
	uppernext = 2
	if ($1 < 0)
	    upper = lower + 99
	else
	    upper = $1
	endif
	print ("Range:",lower,"to",upper)
    }
    else if ($1 == "#ROOMS" ) isok = 1
    else if ($1 == "#RESETS") isok = 0
    else if (isok = 1)
    {
	if ($1 == "D0") {current += 1
		hold = 0}
	else if ($1 == "D1") {current += 1
		hold = 0}
	else if ($1 == "D2") {current += 1
		hold = 0}
	else if ($1 == "D3") {current += 1
		hold = 0}
	else if ($1 == "D4") {current += 1
		hold = 0}
	else if ($1 == "D5") {current += 1
		hold = 0}
	else if ($1 == "S") hold = 1
	else if ($1 == "E") hold = 1
	else if (hold == 0)
	{
	    if ($2 != "")
	    {
		a[current] = $1
		b[current] = $2
		c[current] = $3
		line[current] = count - 1
	    }
	}
    }
    count += 1
}
END	{
    for(i = 1; i<=current; i++)
    {
	if (c[i] != -1 && (c[i] < lower || c[i] > upper))
	    print ("Line",line[i],"leads to room vnum",c[i])
    }
}
