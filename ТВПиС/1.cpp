int N = 4;
int R = rand(9999);
int secondN = N;

int arr[4];
arr[2] = 5;
int i = R;
int count = 0;
while (i >= 0)
{
	count++;
	i--;
}
output("N = ");
output(N);
output("\n");

output("R = ");
output(R);
output("\n");

output("count = ");
output(count);
output("\n");

output("arr[2] = ");
output(arr[2]);
output("\n");

input(i);

if (i > 0)
{
	output(i);
	output(" > 0");
}
else
{
	output(i);
	output(" <= 0");
}

input(i);