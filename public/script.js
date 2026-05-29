console.log("Welcome to the Finance Tracker!");

UpdateUi();

async function add(str, num) {

    await fetch("/transactions", {
        method: "POST",
        body: JSON.stringify({
            "description": str,
            "amount": num
        }),
        headers: {"Content-Type": "application/json"}
    });
}

async function UpdateUi() {
    const list = document.getElementById("transaction");
    const Balance = document.getElementById("balance");
    let total_balance = 0;

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