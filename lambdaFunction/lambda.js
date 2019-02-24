exports.handler = async (event) => {    
    var responceBodySample = {
        train:
        [
            "08:17",
            "08:20",
            "08:30"
        ]
    }
    var responceBody = responceBodySample;
    const response = {
        statusCode: 200,
        body: JSON.stringify(responceBody),
    };
    return response;
};
