let arr = [20, 33, 1, 'Человек', 2, 3];
let result = 0;

for (let i = 0; i < arr.length;i++){
	if(typeof(arr[i]) == 'number'){
		result += Math.pow(arr[i], 3);
	}
}

console.log(Math.sqrt(result));