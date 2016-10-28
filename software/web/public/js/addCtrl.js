// Creates the addCtrl Module and Controller. Note that it depends on the 'geolocation' module and service.
var addCtrl = angular.module('addCtrl', ['geolocation']);
addCtrl.controller('addCtrl', function($scope, $http, $rootScope, geolocation, gservice){
    
    //Initialise variables
    $scope.formData = {};
    // Set initial coordinates to the center of the US
    $scope.formData.paddock = 0;
    $scope.formData.point = 0;
    $scope.formData.version = 0;
    $scope.formData.latitude = -37.911751;
    $scope.formData.longitude = 145.138537;
    $scope.animals = [];

    $(document).ready(function() {
        // Get User's actual coordinates based on HTML5 at window load
        geolocation.getLocation().then(function (data) {
            // Set the latitude and longitude equal to the HTML5 coordinates
            var coords = {lat: data.coords.latitude, long: data.coords.longitude};

            // Display coordinates in location textboxes rounded to three decimal points
            $scope.formData.longitude = parseFloat(coords.long).toFixed(6);
            $scope.formData.latitude = parseFloat(coords.lat).toFixed(6);
            // Set the latitude and longitude equal to the HTML5 coordinates
            $.getJSON('/tracking/list', function (data) {
                gservice.refreshAnimals($scope.formData.latitude, $scope.formData.longitude, data);
            });
        });


        $.getJSON('/tracking/list', function (data) {
            gservice.refreshAnimals($scope.formData.latitude, $scope.formData.longitude, data);
        });

        var now = new Date();
        now.setMilliseconds(0);
        now.setSeconds(0);
        $scope.formData.newestDate = now;
        //now.setMonth(now.getMonth()-1);
        $scope.formData.oldestDate = now;
        $scope.animals[0] = {name: "All", id: null};
        $.getJSON('/animals/list', function (data) {
            Array.from(data).forEach(function (n, i) {
            //data.forEach(function (n, i) {

                $scope.animals[i + 1] = {name: n.name, id: n._id};
            });
        });
        $scope.selectedAnimal = $scope.animals[0];
        console.log($scope.animals);
        
    });
    
//    // Functions
//    // ----------------------------------------------------------------------------
//    
//    
//    function updateList(data) {
//        var fencePoints = [];
//        $scope.fencePoints = data;
//        $scope.formData.point = $scope.fencePoints.length;
//        $scope.formData.version = $scope.fencePoints[0].version;  //** Has console error if there is no entries in database
//        gservice.refresh($scope.formData.latitude, $scope.formData.longitude, false);
//    }
//    
//    // Get coordinates based on mouse click. When a click event is detected....
//    $rootScope.$on("clicked", function () {
//
//        // Run the gservice functions associated with identifying coordinates
//        $scope.$apply(function () {
//            $scope.formData.latitude = parseFloat(gservice.clickLat).toFixed(6);
//            $scope.formData.longitude = parseFloat(gservice.clickLong).toFixed(6);
//        });
//    });
//    
//    $scope.refresh = function () {
//        $.getJSON('/tracking/list', function (data) {
//            console.log(data);
//            gservice.refreshAnimals($scope.formData.latitude, $scope.formData.longitude, data);
//        });
//    };
//    
//    
//    // Take query parameters and incorporate into a JSON queryBody
//    $scope.queryAnimals = function () {
//        var queryBody = {};
//        // Assemble Query Body
//        queryBody = {
//            oldestDate: $scope.formData.oldestDate,
//            newestDate: $scope.formData.newestDate,
//            animalID  : $scope.selectedAnimal.id
//        };
//        // Post the queryBody to the /query POST route to retrieve the filtered results
//        $http.post('/tracking/list', queryBody)
//            
//            // Store the filtered results in queryResults
//            .success(function (queryResults) {
//                // Pass the filtered results to the Google Map Service and refresh the map
//                gservice.refreshAnimals($scope.formData.latitude, $scope.formData.longitude, queryResults);
//
//                // Count the number of records retrieved for the panel-footer
//                $scope.queryCount = queryResults.length;
//            })
//            .error(function (queryResults) {
//                console.log('Error ' + queryResults);
//            });
//    };
});