console.log("Welcome to the Finance Tracker!");

// let transaction = JSON.parse(localStorage.getItem("transactions")) || [];
// let total_balance = transaction.reduce((acc, t) => acc + t.income, 0);

UpdateUi();
// function save() {
//     localStorage.setItem("transactions", JSON.stringify(transaction));
// }

async function add(str, num) {

    await fetch("/transactions", {
        method: "POST",
        body: JSON.stringify({
            "description": str,
            "amount": num
        }),
        headers: {"Content-Type": "application/json"}
    });
    
    // let t = {
    //     name: str,
    //     income: num
    // }
    // total_balance += num;
    // transaction.push(t);
    // save();
}

// function display() {
//     console.log(transaction);
// }

// function del(index) {



    // total_balance -= transaction[index].income;
    // transaction.splice(index, 1);
    // save();
// }

async function UpdateUi() {
    const list = document.getElementById("transaction");
    const Balance = document.getElementById("balance");
    let total_balance = 0;
    // list.innerHTML = "";
    // transaction.forEach((t_n, idx) => {    
    //     const t = document.createElement("li");
    //     t.innerText = `${t_n.name}: ${t_n.income}`;

    //     if (t_n.income >= 0) {
    //         t.classList.add("income");
    //     } else {
    //         t.classList.add("expense");
    //     }

    //     const button = document.createElement("button");
    //     button.innerHTML = "Delete";
    //     button.onclick = () => handleDelete(idx);
    //     t.appendChild(button);
    //     list.append(t);
    // });

    try {
        const res = await fetch("/transactions", {
            method: "GET"
        });
        const data = await res.json();
        list.innerHTML = "";
        data.forEach(transaction => {
            const t = document.createElement("li");
            t.innerText = `${transaction.description}: ${transaction.amount}`;

            if (transaction.amount >= 0) {
                t.classList.add("income");
            } else {
                t.classList.add("expense");
            }
            total_balance += transaction.amount;

            const button =  document.createElement("button");
            button.innerHTML = "Delete";
            button.onclick = () => handleDelete(transaction.id);
            t.appendChild(button);
            list.append(t);
        });
        Balance.innerText = total_balance;
    } catch (error) {
        console.error(error);
    }
    
}
async function handleAdd() {
    const name = document.getElementById("name").value;
    const amount = parseFloat(document.getElementById("amount").value);
    if (name && !isNaN(amount)) {

        try {
            await add(name, amount)
        } catch (error) {
            console.error(error)
        } finally {
            document.getElementById("name").value = null;
            document.getElementById("amount").value = null;
            UpdateUi();
        }

    } else {
        console.error("Missing Fields");
        alert("Please enter both a name and a valid amount.");
    }
}

async function handleDelete(idx) {
    // del(idx);
    try {
        const res = await fetch(`/transactions/${idx}`, {
            method: "DELETE",
        });
        if (!res.ok) {
            throw new Error(`Delete Error: ${res.status}`)
        }
        const data = await res.json();
        alert(data.message);
        console.log(data);
    } catch (error) {
        console.log(error);
        console.error(error);
        alert(error);
    } finally {
        UpdateUi();
    }
   
}