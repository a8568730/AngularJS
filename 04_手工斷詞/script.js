var app1 = angular.module("app1",[]);

app1.controller("showController",function($scope){
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
    /*var mainInfo = null;
    $http.get('答案.json').success(function(data) {
        mainInfo = data;
    });*/
    //step1: "\u8a5e\u7d44" -> "\u8a5e \u7d44"
    $scope.input = unescape(JSON.parse('"\u8a5e \u7d44 \u7d9c \u5408 \u6a19 \u97f3"'));
    //step2: "\u8a5e \u7d44" -> "詞組"
    //step3: "詞組" -> "詞"、"組"
    $scope.str3="\u9023\u5b57\u97f3";
    	/*
    $scope.str2 = [];
    angular.forEach(input, function(word){
    	$scope.str2.push({word:""});
    });*/
    $scope.chinese = '[[{"aa": [{"\u578b\u9ad4": "\u6885", "\u81fa\u7f85\u6578\u5b57\u8abf": "mui5"}, {"\u578b\u9ad4": "\u5c71", "\u81fa\u7f85\u6578\u5b57\u8abf": "san1"}, {"\u578b\u9ad4": "\u7334", "\u81fa\u7f85\u6578\u5b57\u8abf": "kau5"}, {"\u578b\u9ad4": "\u707d", "\u81fa\u7f85\u6578\u5b57\u8abf": "tsai1"}, {"\u578b\u9ad4": "\u9109", "\u81fa\u7f85\u6578\u5b57\u8abf": "hiong1"}, {"\u578b\u9ad4": "\u516c", "\u81fa\u7f85\u6578\u5b57\u8abf": "kong1"}, {"\u578b\u9ad4": "\u6240", "\u81fa\u7f85\u6578\u5b57\u8abf": "soo2"}, {"\u578b\u9ad4": "\u5029", "\u81fa\u7f85\u6578\u5b57\u8abf": "tshiann3"}, {"\u578b\u9ad4": "\u4eba", "\u81fa\u7f85\u6578\u5b57\u8abf": "lang5"}, {"\u578b\u9ad4": "\u8d95", "\u81fa\u7f85\u6578\u5b57\u8abf": "kuann2"}, {"\u578b\u9ad4": "\u8d70", "\u81fa\u7f85\u6578\u5b57\u8abf": "tsau2"}, {"\u578b\u9ad4": "\u7334", "\u81fa\u7f85\u6578\u5b57\u8abf": "kau5"}, {"\u578b\u9ad4": "\u5c71", "\u81fa\u7f85\u6578\u5b57\u8abf": "san1"}], "\u9023\u5b57\u97f3": "mui5 san1 kau5 tsai1 hiong1-kong1-soo2 tshiann3-lang5 kuann2-tsau2 kau5-san1"}]]';
    $scope.chinese2 = JSON.parse($scope.chinese);
    /*$scope.chinese = [
            [{
            	"\u8a5e\u7d44\u7d9c\u5408\u6a19\u97f3": [{
            		"\u578b\u9ad4": "\u6885", 
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "mui5"
            	}, {
            		"\u578b\u9ad4": "\u5c71", 
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "san1"
            	}, {
            		"\u578b\u9ad4": "\u7334", 
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "kau5"
            	}, {
            		"\u578b\u9ad4": "\u707d", 
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "tsai1"
            	}, {
            		"\u578b\u9ad4": "\u9109",
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "hiong1"
            	}, {
            		"\u578b\u9ad4": "\u516c",
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "kong1"
            	}, {
            		"\u578b\u9ad4": "\u6240",
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "soo2"
            	}, {
            		"\u578b\u9ad4": "\u5029",
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "tshiann3"
            	}, {
            		"\u578b\u9ad4": "\u4eba",
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "lang5"
            	}, {
            		"\u578b\u9ad4": "\u8d95",
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "kuann2"
            	}, {
            		"\u578b\u9ad4": "\u8d70",
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "tsau2"
            	}, {
            		"\u578b\u9ad4": "\u7334",
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "kau5"
            	}, {
            		"\u578b\u9ad4": "\u5c71",
            		"\u81fa\u7f85\u6578\u5b57\u8abf": "san1"
            	}], 
            	
            	"\u9023\u5b57\u97f3":"mui5 san1 kau5 tsai1 hiong1-kong1-soo2 tshiann3-lang5 kuann2-tsau2 kau5-san1"
            }]];*/
});