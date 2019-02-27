"use strict";
process.env.TZ = 'Asia/Tokyo';

let fs = require("fs");

const file_path_Weekend = "data/Tsurumiono_Weekend.csv";
const file_path_Weekday = "data/Tsurumiono_Weekday.csv";

exports.handler = async (event) => {    

    let date_now = new Date();
    let file_path;
    if(date_now.getDay() == 0 || date_now.getDay() == 6){
        file_path = file_path_Weekend;
    }
    else{
        file_path = file_path_Weekday;
    }
    
    const file = fs.readFileSync(file_path , {encoding: "utf-8"});

    let train_array = [];
    
    //CSVを読み出して、配列に時間文字列を入れていく
    file.split('\n').forEach((line,index) => {
        line.split(',').forEach( (item) => {
            if(item == ''){
                return;
            }
            let trainDate = new Date();
            trainDate.setHours((index+1));
            trainDate.setMinutes(item);
            trainDate.setSeconds(0);
            console.log(trainDate.toString());
            train_array.push(trainDate.toString());
        })
    });

    let result_array = [];
    const train_count_limit = 3;

    console.log("---now---");
    console.log(new Date().toString());
    console.log("---now---");



    train_array.forEach( (item,index) => {
        if (result_array.length >= train_count_limit) {
            return;
        } 
    
        if(new Date(item).getTime() > new Date().getTime()){
            // result_array.push(new Date(item).toString());
            let result_str = formatDate(new Date(item), 'hh:mm');
            result_array.push(result_str);
            // console.log(new Date(item).toString());
            console.log(result_str);
        }
    })

    let responceBody = {
        train: result_array
    }

    // let responceBodySample = {
    //     train:
    //     [
    //         "08:17",
    //         "08:20",
    //         "08:30"
    //     ]
    // }

    const response = {
        statusCode: 200,
        body: JSON.stringify(responceBody),
    };
    return response;
};

function formatDate(date , format) {
    format = format.replace(/YYYY/g, date.getFullYear());
    format = format.replace(/MM/g, ('0' + (date.getMonth() + 1)).slice(-2));
    format = format.replace(/DD/g, ('0' + date.getDate()).slice(-2));
    format = format.replace(/hh/g, ('0' + date.getHours()).slice(-2));
    format = format.replace(/mm/g, ('0' + date.getMinutes()).slice(-2));
    format = format.replace(/ss/g, ('0' + date.getSeconds()).slice(-2));
    return format;
}