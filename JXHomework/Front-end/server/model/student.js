const mongoose = require('mongoose');

const studentSchema = new mongoose.Schema({
    name: {
        type: String,
        required: true,
        minlength: 2,
        maxlength: 10
    },
    age: {
        type:  Number,
        min : 16,
        max: 100
    },
    password : String,
    email : String,
    courses: [String]
});

const Student = new mongoose.model('Student', studentSchema);
module.exports = Student;