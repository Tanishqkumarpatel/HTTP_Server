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

            const del_button =  document.createElement("button");
            del_button.innerHTML = "Delete";
            del_button.id = "delete-btn";
            del_button.onclick = () => handleDelete(transaction.id);
            
            const edit_button = document.createElement("button");
            edit_button.innerHTML = "Edit";
            edit_button.id = "edit-btn";
            edit_button.onclick = () => handleEdit(transaction.id);

            const btn_group = document.createElement("div");
            btn_group.appendChild(edit_button);
            btn_group.appendChild(del_button);
            
            t.appendChild(btn_group);
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

async function handleEdit(id) {
    
    try {
        const res = await fetch(`/transactions/${id}`, {
            method: "GET",
        });
        if (!res.ok) {
            throw new Error(`Get Error: ${res.status}`)
        }
        const data = await res.json()
        let description = document.getElementById("modal-name")
        let amount = document.getElementById("modal-amount")
        description.value = data.description
        amount.value = data.amount
        document.getElementById("modal-overlay").style.display = "flex"
        document.getElementById("modal-box").dataset.id = id;
    } catch (error) {
        console.log(error)
        console.error(error);
    }
}

async function handleSave() {

    const description =  document.getElementById("modal-name").value
    const amount = parseFloat(document.getElementById("modal-amount").value)
    const id =  document.getElementById("modal-box").dataset.id
    if (!description && isNaN(amount)) {
        alert("Please enter both a name and a valid amount.");
    }
    try {
        const res = await fetch(`/transactions/${id}`, {
            method: "PUT",
            body: JSON.stringify({
                "description": description,
                "amount": amount
            }),
            headers: {"Content-Type": "application/json"}
        })
        if (!res.ok) {
            throw new Error(`PUT Error: ${res.status}`)
        }
        alert("Edit successful")
    } catch (error) {
        alert(error)
        console.error(error)
    } finally {
        document.getElementById("modal-overlay").style.display = "none"
        UpdateUi()
    }
}

function handleCancel() {
    document.getElementById("modal-overlay").style.display = "none"
}