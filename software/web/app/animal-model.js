// Pulls Mongoose dependency for creating schemas
var mongoose    = require('mongoose');
var Schema      = mongoose.Schema;


//var AnimalPositionSchema = new Schema({
//    location: {type: [Number], required: true}, // [Long, Lat]
//    alerts: {type: Number, required: true},
//    shocks: {type: Number, required: true},
//    sent_at: {type: Date, required: true}
//});

// Creates a User Schema. This will be the basis of how user data is stored in the db
var AnimalSchema = new Schema({
    RF_ID: {type: Number, required: true},
    name: {type: String, required: true},
    paddock: {type: Number, required: true},
    born: {type: Date, required: true},
    breed: {type: String, required: true}, 
    gender: {type: String, required: true},
    comments: {type: String, required: true},
    created_at: {type: Date, default: Date.now},
    updated_at: {type: Date, default: Date.now}
});



// Sets the created_at parameter equal to the current time
AnimalSchema.pre('save', function(next){
    now = new Date();
    this.updated_at = now;
    if(!this.created_at) {
        this.created_at = now
    }
    next();
});

// Indexes this schema in 2dsphere format (critical for running proximity searches)
AnimalSchema.index({location: '2dsphere'});

// Exports the UserSchema for use elsewhere. Sets the MongoDB collection to be used as: "scotch-users" (adds the s)
module.exports = mongoose.model('animal', AnimalSchema);

