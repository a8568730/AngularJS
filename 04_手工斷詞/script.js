var app1 = angular.module("app1",[]);

app1.controller("showController",function($scope,$filter){
    $scope.str = [{
                word:"我",
                clicked:false
            },{
                word:"姓",
                clicked:false
            },{
                word:"豬",
                clicked:false
            }
        ];
    
    /*印出萬國碼方法 1*/
    //$scope.input = unescape(JSON.parse('"\u8a5e \u7d44 \u7d9c \u5408 \u6a19 \u97f3"'));
    /*印出萬國碼方法 2*/
    //$scope.str3="\u9023\u5b57\u97f3";
    
    //初始化單行原有句子
    $scope.chinese = '[[{"\u8a5e\u7d44\u7d9c\u5408\u6a19\u97f3": [{"\u578b\u9ad4": "\u6885", "\u81fa\u7f85\u6578\u5b57\u8abf": "mui5"}, {"\u578b\u9ad4": "\u5c71", "\u81fa\u7f85\u6578\u5b57\u8abf": "san1"}, {"\u578b\u9ad4": "\u7334", "\u81fa\u7f85\u6578\u5b57\u8abf": "kau5"}, {"\u578b\u9ad4": "\u707d", "\u81fa\u7f85\u6578\u5b57\u8abf": "tsai1"}, {"\u578b\u9ad4": "\u9109", "\u81fa\u7f85\u6578\u5b57\u8abf": "hiong1"}, {"\u578b\u9ad4": "\u516c", "\u81fa\u7f85\u6578\u5b57\u8abf": "kong1"}, {"\u578b\u9ad4": "\u6240", "\u81fa\u7f85\u6578\u5b57\u8abf": "soo2"}, {"\u578b\u9ad4": "\u5029", "\u81fa\u7f85\u6578\u5b57\u8abf": "tshiann3"}, {"\u578b\u9ad4": "\u4eba", "\u81fa\u7f85\u6578\u5b57\u8abf": "lang5"}, {"\u578b\u9ad4": "\u8d95", "\u81fa\u7f85\u6578\u5b57\u8abf": "kuann2"}, {"\u578b\u9ad4": "\u8d70", "\u81fa\u7f85\u6578\u5b57\u8abf": "tsau2"}, {"\u578b\u9ad4": "\u7334", "\u81fa\u7f85\u6578\u5b57\u8abf": "kau5"}, {"\u578b\u9ad4": "\u5c71", "\u81fa\u7f85\u6578\u5b57\u8abf": "san1"}], "\u9023\u5b57\u97f3": "mui5 san1 kau5 tsai1 hiong1-kong1-soo2 tshiann3-lang5 kuann2-tsau2 kau5-san1"}]]';
    var replaced = $scope.chinese.replace(/\u9023\u5b57\u97f3/g,"string")
    					  .replace(/\u8a5e\u7d44\u7d9c\u5408\u6a19\u97f3/g,"words")
    					  .replace(/\u578b\u9ad4/g,"shape")
    					  .replace(/\u81fa\u7f85\u6578\u5b57\u8abf/g,"tone");
    $scope.chinese2 = JSON.parse(replaced);
    
    angular.forEach($scope.chinese2[0][0].words, function(word){
    	word.clicked = true;
    });
    
    //取出原有句子的斷詞資訊 hiong1-kong1-soo2 -> ["hiong1-","kong1-","soo2"]
    $scope.string = $scope.chinese2[0][0].string.replace(/-/g,"- ").split(" ");
    angular.forEach($scope.string, function(tone, index){
    	if(tone.slice(-1) == "-")
    		$scope.chinese2[0][0].words[index].clicked=false;
    });
   
    //輸出手工斷詞後的新句子
});

