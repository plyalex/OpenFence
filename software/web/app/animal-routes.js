var Animal            = require('./animal-model.js');


// Opens App Routes
module.exports = function(app) {
    
    
    // GET Routes
    // --------------------------------------------------------
    // Retrieve records for all users in the db
    app.get('/tracking/list', function(req, res){

        // Uses Mongoose schema to run the search (empty conditions)
        var query =Animal.find({});
        query.setOptions({sort: "animal"});
        query.exec(function(err, animals){
            if(err)
                res.send(err);

            // If no errors are found, it responds with a JSON of all users
            res.json(animals);
        });
    });
    
        // Retrieves JSON records for all users who meet a certain set of query conditions
    app.post('/tracking/list', function(req, res){

        // Grab all of the query parameters from the body.
        var oldestDate      = req.body.oldestDate;
        var newestDate      = req.body.newestDate;
        var animalID        = req.body.animalID;

        // Opens a generic Mongoose Query. Depending on the post body we will...
        var query = Animal.find({});

 
        if(oldestDate){
            query = query.where('sent_at').gte(oldestDate);
        }

        // ...include filter by Max Age
        if(newestDate){
            query = query.where('sent_at').lte(newestDate);
        }

        // ...include filter for HTML5 Verified Locations
        if(animalID){
            query = query.where('animal').equals(animalID);
        }
        console.log(animalID);
        // Execute Query and Return the Query Results
        query.exec(function(err, animals){
            if(err)
                res.send(err);

            // If no errors, respond with a JSON of all users that meet the criteria
            res.json(animals);
        });
    });
    
    // Provides method for saving new points in the db
    app.post('/tracking/add', function(req, res){
        console.log("Adding a new Point")
        // Creates a new Animal point based on the Mongoose schema and the post body.
        var newpoint = new Animal(req.body);
        console.log(req.body)
        // New Fence is saved in the db.
        newpoint.save(function(err){
            if(err)
                res.send(err);

            // Uses Mongoose schema to run the search (empty conditions)
            var query = Animal.find({});
            query.setOptions({sort: "order"});
            query.exec(function(err, animals){
            if(err)
                res.send(err);

            // If no errors are found, it responds with a JSON of all users
            res.json(animals);
            });
        });
    });
};  