Trigger greet_prog 80~
if ispc($n)
	beg $n
	say Spare some gold?
endif
~
Trigger bribe_prog 10000~
dance $n
french $n
~
Trigger bribe_prog 1000~
say Oh my GOD!  Thank you! Thank you!
french $n
~
Trigger bribe_prog 100~
say Wow!  Thank you! Thank you!
~
Trigger bribe_prog 1~
thank $n
~
Trigger fight_prog 20~
say Help!  Please somebody help me!
if rand(50)
	say Ouch!
else
	say I'm bleeding.
endif
~
Trigger death_prog 50~
if rand(50)
	mpechoaround $i $I says 'Now I go to a better place.'
else
	mpechoaround $i $I says 'Forgive me God for I have sinned...'
endif
~
End

