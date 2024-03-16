window.addEventListener('message', function (event) {
    // 确认消息来源是父页面
    if (event.source === parent) {
        // 更新input的内容为收到的文件名
        var inputElement = document.getElementById('fileNameInput');
        inputElement.value = event.data;
    }
});

function screen() {
    var inputElement = document.getElementById('fileNameInput');
    console.log(inputElement.value);
    var baseUrl = "http://192.168.2.150:9090/screen";
    var url = baseUrl + "/" + inputElement.value;
    fetch(url, {
        method: 'GET',
    });
}