function validateForm() {
    var username = document.getElementById('username').value;
    var password = document.getElementById('password').value;

    if (username === 'admin' && password === '123456') {
        return true;  // 允许提交表单，跳转页面
    } else {
        alert('用户名或密码错误！');
        return false; // 阻止提交表单
    }
}