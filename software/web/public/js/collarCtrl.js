var collarCtrl = angular.module('collarCtrl',[]);

collarCtrl.controller('collarCtrl', function($scope, $http, $rootScope){
    
    var now = new Date;
    now.setMilliseconds(0);
    now.setSeconds(0);
    
    $scope.formData = {};
    $scope.formData.name="";
    $scope.formData.RF_ID=1;
    $scope.formData.paddock=0;
    $scope.formData.born=now;
    $scope.formData.breed="Cow";
    $scope.formData.gender="Female";
    $scope.formData.testing=0;
    $scope.formData.comments="";
    $scope.formData.colour="#FF00FF";
    $scope.formData.distthresh=2;
    $scope.formData.motionthresh=3;
    $scope.formData.magbias0=3;
    $scope.formData.magbias1=3;
    $scope.formData.magbias2=3;
    
    $scope.selectedRow=null;
    
    // jQuery AJAX call for JSON
    $.getJSON( '/animals/list', function( data ) {
        updateList(data);
    });
    
//        $scope.animals=[];
//    $scope.animals[0]={name: "All", id: null};
//    $.getJSON( '/animals/list', function( data ) {
//        data.forEach(function(n, i){
//            $scope.animals[i+1]={name: n.name, id: n._id};
//        });
//    });
//    $scope.selectedAnimal = $scope.animals[0];
    
    // Functions
    // ----------------------------------------------------------------------------
    
    
    function updateList(data){
        var deviceList = [];
        $scope.deviceList = data;
        console.log($scope.deviceList);
    };
    
    $scope.updatelist = function() {
        $.getJSON( '/animals/list', function( data ) {
            updateList(data);
        });
    };
    
    $scope.view = function(data){
        console.log(data);
        $scope.selectedRow=data;
        var jsondata={name:$scope.deviceList[data].name};
        $http.post('/animals/query', jsondata)
            .success(function (data) {
                var data1 = data[0];
                console.log(data);
                var born = new Date(data1.born);
                $scope.formData.name=data1.name;
                $scope.formData.RF_ID=data1.New_RF_ID;
                $scope.formData.paddock=data1.paddock;
                $scope.formData.born=born;
                $scope.formData.breed=data1.breed;
                $scope.formData.gender=data1.gender;
                $scope.formData.testing=data1.testing;
                $scope.formData.comments=data1.comments;
                $scope.formData.colour=data1.colour;
                $scope.formData.distthresh=data1.distthresh;
                $scope.formData.motionthresh=data1.motionthresh;
                $scope.formData.magbias0=data1.magbias0;
                $scope.formData.magbias1=data1.magbias1;
                $scope.formData.magbias2=data1.magbias2;
            })  
            .error(function (data) {
                //console.log('Error: ' + data);
                //updateList();
            });
    };
    
    $scope.saveAs = function(){
        // Grabs all of the text box fields
        var animalData = {
                RF_ID: $scope.formData.RF_ID,
                New_RF_ID: $scope.formData.RF_ID,
                name: $scope.formData.name,
                paddock: $scope.formData.paddock,
                born: $scope.formData.born,
                breed: $scope.formData.breed,
                gender: $scope.formData.gender,
                comments: $scope.formData.comments,
                colour: $scope.formData.colour,
                testing: $scope.formData.testing,
                distthresh: $scope.formData.distthresh,
                motionthresh: $scope.formData.motionthresh,
                magbias0: $scope.formData.magbias0,
                magbias1: $scope.formData.magbias1,
                magbias2: $scope.formData.magbias2
        };

        // Saves the user data to the db
        $http.post('/animals/add', animalData)
            .success(function (data) {

                // Once complete, clear the form (except location)
                updateList(data);
            
                // Refresh the map with new data
                //gservice.refresh($scope.formData.latitude, $scope.formData.longitude);
                
            })
            .error(function (data) {
                console.log('Error: ' + data);
            });
    };
    
    $scope.update = function(){
        var animalData = {
                New_RF_ID: $scope.formData.RF_ID,
                name: $scope.formData.name,
                paddock: $scope.formData.paddock,
                born: $scope.formData.born,
                breed: $scope.formData.breed,
                gender: $scope.formData.gender,
                comments: $scope.formData.comments,
                colour: $scope.formData.colour,
                testing: $scope.formData.testing,
                distthresh: $scope.formData.distthresh,
                motionthresh: $scope.formData.motionthresh,
                magbias0: $scope.formData.magbias0,
                magbias1: $scope.formData.magbias1,
                magbias2: $scope.formData.magbias2
        };
        // Saves the user data to the db
        $http.post('/animals/update', animalData)
            .success(function (data) {

                // Once complete, clear the form (except location)
                updateList(data);
            
                // Refresh the map with new data
                //gservice.refresh($scope.formData.latitude, $scope.formData.longitude);
                
            })
            .error(function (data) {
                console.log('Error: ' + data);
            });
    };
});