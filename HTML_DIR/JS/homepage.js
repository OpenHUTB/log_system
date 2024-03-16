
    window.addEventListener('message', function (event) {
        if (event.source === document.querySelector('iframe[src="./content-item-1.html"]').contentWindow) {
            if (event.data.action === 'analyze') {
                // 执行点击 menu-item-2 的操作
                document.getElementById('menu-item-2').click();

                // 设置 iframe 中的 input type=text 的内容为文件名
                var iframe2 = document.querySelector('iframe[src="./content-item-2.html"]');
                // console.log(event.data.fileName);
                // console.dir(iframe2);
                //var inputText = iframe2.contentDocument.getElementById('fileNameInput');
                // inputText.value = "111";
                iframe2.contentWindow.postMessage(event.data.fileName, '*');
                   
            }
        }
    });
function menu_item_onclick(id) {
    let arr1 = ["../IMAG/文件列表.png", "../IMAG/日志处理.png", "../IMAG/数据分析.png"];
    let arr2 = ["../IMAG/文件列表-o.png", "../IMAG/日志处理-o.png", "../IMAG/数据分析-o.png"];
    var menuItems = document.getElementsByClassName("menu-item");
    let index = -1;
    for (var i = 0; i < menuItems.length; i++) {
        menuItems[i].style.color = "rgb(191, 203, 217)";
        menuItems[i].getElementsByTagName("img")[0].src = arr1[i];
        if (id == menuItems[i]) {
            index = i;
            id.style.color = "rgb(250,207,53)";
            id.getElementsByTagName("img")[0].src = arr2[i];
        }
    }
    let arr3 = document.querySelectorAll(".content-item");
    for (var i = 0; i < arr3.length; i++) {
        arr3[i].style.display = "none";
        if (i == index)
            arr3[i].style.display = "block";
    }
    if (index == 0) {
        const iframe = document.getElementById('content-item-0').querySelector('iframe');
        iframe.src = "http://192.168.2.150:9090";
    }
}
