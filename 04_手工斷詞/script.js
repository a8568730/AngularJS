var app1 = angular.module("app1", ['ngRoute'])
					.config(function($locationProvider) {
					    $locationProvider.html5Mode(true);
					});//add then successfully get url 

app1.controller("showController",["$scope","$location","$http","$q",function($scope,$location,$http,$q){
    /* 印出萬國碼方法 1 */
    //$scope.input = unescape(JSON.parse('"\u8a5e \u7d44 \u7d9c \u5408 \u6a19 \u97f3"'));
    /* 印出萬國碼方法 2 */
    //$scope.str3="\u9023\u5b57\u97f3";
    /* 秀出json資料的方法 */
    //JSON.stringify(jsondata)
    
	var searchObject = $location.search();
    $scope.mainInfo = $http.get("./答案.txt").success(function(data) {
        //1.讀入十句        
    	$scope.input10 = data.slice(searchObject.aaaa,searchObject.aaaa + 10);
    	
	    //2.初始化每一句要斷詞的句子 (anguarJS無法顯示{{中文變數}})
    	angular.forEach($scope.input10,function(每一句, 索引值){
    		
    		每一句[0][0].words = 每一句[0][0].詞組綜合標音;
    		每一句[0][0].string = 每一句[0][0].連字音;
    		
    		var thread = 
    		angular.forEach(每一句[0][0].words, function(word){
    			word.shape = word.型體;
    			word.tone = word.臺羅數字調;
    			word.clicked = true;
    	    });
    		//不可以寫$q.all(thread).then(取出斷詞資訊(句子, 詞組));
    		//因為這樣它會先算出"取出斷詞資訊(句子, 詞組)"作為then的參數, 而不是進入then之後做的事情。
        	$q.all(thread).then(function(){
        		取出斷詞資訊(每一句[0][0].string, 每一句[0][0].words);
        	});
		});	
	
    	//取出原有句子的斷詞資訊 hiong1-kong1-soo2 -> ["hiong1-","kong1-","soo2"]
    	function 取出斷詞資訊(句子, 詞組){
		    angular.forEach(句子.replace(/-/g,"- ").split(" "), function(tone, index){
		    	if(tone.slice(-1) == "-")
		    		詞組[index].clicked=false;
		    });
	    };
	    
	    /*單行json資料，而且是字串的匯入初始方法*/
	    /* $scope.chinese = '[[{"\u8a5e\u7d44\u7d9c\u5408\u6a19\u97f3": [{"\u578b\u9ad4": "\u6885", "\u81fa\u7f85\u6578\u5b57\u8abf": "mui5"}, {"\u578b\u9ad4": "\u5c71", "\u81fa\u7f85\u6578\u5b57\u8abf": "san1"}, {"\u578b\u9ad4": "\u7334", "\u81fa\u7f85\u6578\u5b57\u8abf": "kau5"}, {"\u578b\u9ad4": "\u707d", "\u81fa\u7f85\u6578\u5b57\u8abf": "tsai1"}, {"\u578b\u9ad4": "\u9109", "\u81fa\u7f85\u6578\u5b57\u8abf": "hiong1"}, {"\u578b\u9ad4": "\u516c", "\u81fa\u7f85\u6578\u5b57\u8abf": "kong1"}, {"\u578b\u9ad4": "\u6240", "\u81fa\u7f85\u6578\u5b57\u8abf": "soo2"}, {"\u578b\u9ad4": "\u5029", "\u81fa\u7f85\u6578\u5b57\u8abf": "tshiann3"}, {"\u578b\u9ad4": "\u4eba", "\u81fa\u7f85\u6578\u5b57\u8abf": "lang5"}, {"\u578b\u9ad4": "\u8d95", "\u81fa\u7f85\u6578\u5b57\u8abf": "kuann2"}, {"\u578b\u9ad4": "\u8d70", "\u81fa\u7f85\u6578\u5b57\u8abf": "tsau2"}, {"\u578b\u9ad4": "\u7334", "\u81fa\u7f85\u6578\u5b57\u8abf": "kau5"}, {"\u578b\u9ad4": "\u5c71", "\u81fa\u7f85\u6578\u5b57\u8abf": "san1"}], "\u9023\u5b57\u97f3": "mui5 san1 kau5 tsai1 hiong1-kong1-soo2 tshiann3-lang5 kuann2-tsau2 kau5-san1"}]]';
	    var replaced = $scope.chinese.replace(/\u9023\u5b57\u97f3/g,"string")
	    					  .replace(/\u8a5e\u7d44\u7d9c\u5408\u6a19\u97f3/g,"words")
	    					  .replace(/\u578b\u9ad4/g,"shape")
	    					  .replace(/\u81fa\u7f85\u6578\u5b57\u8abf/g,"tone");
	    $scope.chinese2 = JSON.parse(replaced);
	    
	    angular.forEach($scope.chinese2[0][0].words, function(word){
	    	word.clicked = true;
	    });*/
    
	    
	    //3.存入手工斷詞後的十句
	    var outurl = "./output.json";
	    
	    $scope.restore = function(){
	    	var 輸出所有句 = [];
	    	var 總長度 = $scope.input10.length;
	    	for(var i=0; i<總長度; i++) {
	    		var 每一句 = "";
	    		var 字數 = $scope.input10[i][0][0].words.length;
	    		for(var j=0; j<字數; j++){
	    			var 字 = $scope.input10[i][0][0].words[j];
	    			每一句 += 字.clicked? 字.臺羅數字調 + ' ': 字.臺羅數字調 + '-';  
	    		}
	    		輸出所有句.push(每一句.slice(0,-1));
	    	}
	    	alert(輸出所有句);
	    };  
	    
	    
	});//結束get.success
}]);

