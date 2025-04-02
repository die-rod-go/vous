var[] array;

array <-10;
array <-1;
array <-12;
array <-13;
array <-111;
array <-11;
array <-221;
array <-14;
array <-51;
array <-15;
array <-24;
array <-20;
array <-21;
array <-12;

for(var i = 0; i < 14; i = i + 1)	
{
	for(var j = 0; j < 14 - i - 1; j = j + 1)
	{
		if(array[j] > array[j+1])
		{
			var temp = array[j];
			array[j] = array[j+1];
			array[j+1] = temp;
		}
	}
}

for(var i = 0; i < 14; i = i + 1)
{
	print array[i];
}


