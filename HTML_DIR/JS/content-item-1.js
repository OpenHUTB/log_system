

function uploadFile() {
    const fileInput = document.getElementById('fileInput');
    const file = fileInput.files[0];
    if (file) {
        const formData = new FormData();
        formData.append('file', file);
        fetch('http://192.168.2.150:9090/upload', {
            method: 'POST',
            body: formData
        }).then(response => {
            if (response.ok) {
                alert("上传文件成功");
            }
        })
            .catch(error => {
                console.error('上传文件失败:', error);
            });
    }
}
function click_btn1() {
    let btn1 = document.getElementById("button1");
    let btn2 = document.getElementById("button2");
    btn2.classList.remove('active-btn');
    btn1.classList.add('active-btn');
    let inputArr = document.querySelectorAll(".remote input");
    for (i = 0; i < inputArr.length; ++i) {
        inputArr[i].disabled = true;
        inputArr[i].style.opacity = 0.5;
        inputArr[i].value = '';
    }
    document.querySelector(".remote").style.color = "rgb(218, 218, 214)";
    document.querySelector(".local input").disabled = false;
    document.querySelector(".local input").color = "black";
}
function click_btn2() {
    let btn1 = document.getElementById("button1");
    let btn2 = document.getElementById("button2");
    btn1.classList.remove('active-btn');
    btn2.classList.add('active-btn');
    let inputArr = document.querySelectorAll(".remote input");
    for (i = 0; i < inputArr.length; ++i) {
        inputArr[i].disabled = false;
        inputArr[i].style.opacity = 1;
    }
    document.querySelector(".remote").style.color = "black";
    document.querySelector(".local input").disabled = true;
    document.querySelector(".local input").value = "";
    document.querySelector(".local input").color = "white";

}