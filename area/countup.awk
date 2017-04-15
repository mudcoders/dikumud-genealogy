{
    if ($1 == "#ROOMS" ) isok = 1
    else if (isok == 1)
    {
	isok = 2
	print("This area starts at vnum",$1)
    }
}
