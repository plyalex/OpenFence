// app.js

// Declares the initial angular module "meanMapApp". Module grabs other controllers and services. Note the use of ngRoute.
var app = angular.module('OpenFence', ['addCtrl', 'queryCtrl', 'fenceCtrl','collarCtrl', 'trackingCtrl', 'geolocation', 'gservice', 'ngRoute'])

    // Configures Angular routing -- showing the relevant view and controller when needed.
    .config(function($routeProvider){

        // Overview
        $routeProvider.when('/overview', {
            controller: 'addCtrl', //To Complete
            templateUrl: 'partials/overview.html',

        }).when('/map', {
            controller: 'fenceCtrl',
            templateUrl: 'partials/map.html',
            
        }).when('/collars', {
            controller: 'collarCtrl',  
            templateUrl: 'partials/collars.html',

        }).when('/tracking', {
            controller: 'trackingCtrl',
            templateUrl: 'partials/tracking.html',

        }).when('/statistics', {
            controller: 'queryCtrl', //To Complete
            templateUrl: 'partials/statistics.html',
            
        }).when('/settings', {
            controller: 'queryCtrl', //To Complete
            templateUrl: 'partials/settings.html',
            
            // All else forward to the Join Team Control Panel
        }).otherwise({redirectTo:'/overview'})
    });
