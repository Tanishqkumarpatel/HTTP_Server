console.log("Welcome to the Finance Tracker!");

let transaction = JSON.parse(localStorage.getItem("transactions")) || [];
let total_balance = transaction.reduce((acc, t) => acc + t.income, 0);
UpdateUi();
function save() {
    localStorage.setItem("transactions", JSON.stringify(transaction));
}

function add(str, num) {
    let t = {
        name: str,
        income: num
    }
    total_balance += num;
    transaction.push(t);
    save();
}

function display() {
    console.log(transaction);
}
function del(index) {
    total_balance -= transaction[index].income;
    transaction.splice(index, 1);
    save();
}

function UpdateUi() {
    const list = document.getElementById("transaction");
    const Balance = document.getElementById("balance");
    list.innerHTML = "";
    transaction.forEach((t_n, idx) => {    
        const t = document.createElement("li");
        t.innerText = `${t_n.name}: ${t_n.income}`;

        if (t_n.income >= 0) {
            t.classList.add("income");
        } else {
            t.classList.add("expense");
        }

        const button = document.createElement("button");
        button.innerHTML = "Delete";
        button.onclick = () => handleDelete(idx);
        t.appendChild(button);
        list.append(t);
    });

    Balance.innerText = total_balance;
}
function handleAdd() {
    const name = document.getElementById("name").value;
    const amount = parseFloat(document.getElementById("amount").value);
    if (name && !isNaN(amount)) {
        add(name, amount);
        document.getElementById("name").value = null;
        document.getElementById("amount").value = null;
        UpdateUi();
    } else {
        console.error("Missing Fields");
        alert("Please enter both a name and a valid amount.");
    }
}
function handleDelete(idx) {
    del(idx);
    UpdateUi();
}