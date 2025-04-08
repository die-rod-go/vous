var[] array;
var length;

for(var x = 0; x < 100; x = x + 1)
{
	array <- x;
	length = length + 1;
}

var before = clock();

for(var i = 0; i < length; i = i + 1)	
{
	for(var j = 0; j < length - i - 1; j = j + 1)
	{
		if(array[j] < array[j+1])
		{
			var temp = array[j];
			array[j] = array[j+1];
			array[j+1] = temp;
		}
	}
}
var after = clock();

for(var i = 0; i < length; i = i + 1)
{
	print array[i];
}
print "
";
print after - before;