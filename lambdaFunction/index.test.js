const lambda = require('./index.js');
const event = "";
const context = "";
function callback(error, result) {
  if (typeof error !== 'null') {
    console.error(result);
    process.exit(1);
  }
  console.log(result);
  process.exit(0);
}
lambda.handler(event, context, callback);