// Student Portal Page Logic

document.addEventListener('DOMContentLoaded', () => {
    // 1. Guard check
    const user = API.getUser();
    const token = API.getToken();
    if (!user || !token || user.role !== 'Student') {
        window.location.href = 'index.html';
        return;
    }

    // Set navbar profile
    document.getElementById('navUserName').textContent = user.name;
    const badge = document.getElementById('navUserBadge');
    if (user.membership === 'member') {
        badge.className = 'badge badge-member ms-1';
        badge.textContent = 'Member (20% Off)';
    } else {
        badge.className = 'badge badge-guest ms-1';
        badge.textContent = 'Guest Student';
    }

    document.getElementById('btnLogout').addEventListener('click', () => API.logout());

    // State
    let currentEvents = [];
    let pendingEnrollmentEvent = null;
    let activeEnrollmentId = null;
    let activePaymentId = null;

    // Modals
    const enrollConfirmModal = new bootstrap.Modal(document.getElementById('enrollConfirmModal'));
    const paymentModal = new bootstrap.Modal(document.getElementById('paymentModal'));
    const receiptModal = new bootstrap.Modal(document.getElementById('receiptModal'));
    const myEnrollmentsModal = new bootstrap.Modal(document.getElementById('myEnrollmentsModal'));
    const certificateModal = new bootstrap.Modal(document.getElementById('certificateModal'));

    // Alerts
    function showPageAlert(msg, type = 'danger') {
        const el = document.getElementById('pageAlert');
        const msgEl = document.getElementById('pageAlertMsg');
        el.className = `alert alert-${type} alert-dismissible fade show`;
        msgEl.innerHTML = msg;
        el.classList.remove('d-none');
        window.scrollTo({ top: 0, behavior: 'smooth' });
    }

    // Load Events
    async function loadEvents() {
        try {
            const params = {
                name: document.getElementById('filterName').value.trim(),
                club: document.getElementById('filterClub').value,
                maxFee: document.getElementById('filterMaxFee').value,
                sort: document.getElementById('filterSort').value
            };

            const events = await API.getEvents(params);
            currentEvents = events;
            renderEvents(events);
        } catch (err) {
            showPageAlert('Failed to load events: ' + err.message);
        }
    }

    function renderEvents(events) {
        const grid = document.getElementById('eventsGrid');
        const countEl = document.getElementById('eventsCount');
        grid.innerHTML = '';
        countEl.textContent = `${events.length} available event${events.length === 1 ? '' : 's'}`;

        if (events.length === 0) {
            grid.innerHTML = `
                <div class="col-12 text-center py-5">
                    <i class="bi bi-inbox fs-1 text-muted d-block mb-2"></i>
                    <h6 class="text-muted">No events match your current filter criteria.</h6>
                </div>
            `;
            return;
        }

        events.forEach(ev => {
            const cardCol = document.createElement('div');
            // Responsive Bootstrap Grid: 1 col on phone (col-12), 2 on tablet (col-md-6), 3 on laptop (col-lg-4)
            cardCol.className = 'col-12 col-md-6 col-lg-4';

            let typeBadgeClass = 'badge-workshop';
            if (ev.type === 'Hackathon') typeBadgeClass = 'badge-hackathon';
            else if (ev.type === 'Seminar') typeBadgeClass = 'badge-seminar';
            else if (ev.type === 'CulturalEvent') typeBadgeClass = 'badge-cultural';

            const baseFee = parseFloat(ev.fee);
            const isMember = (user.membership === 'member');
            const payableFee = isMember ? (baseFee * 0.80) : baseFee;

            let extraInfo = '';
            if (ev.trainer) extraInfo = `<div class="text-muted small"><i class="bi bi-person-badge me-1"></i> Trainer: <strong>${ev.trainer}</strong></div>`;
            else if (ev.speaker) extraInfo = `<div class="text-muted small"><i class="bi bi-mic me-1"></i> Speaker: <strong>${ev.speaker}</strong></div>`;
            else if (ev.prize) extraInfo = `<div class="text-muted small"><i class="bi bi-trophy me-1"></i> Prize: <strong>${ev.prize}</strong> (Teams of ${ev.teamSize})</div>`;
            else if (ev.category) extraInfo = `<div class="text-muted small"><i class="bi bi-palette me-1"></i> Category: <strong>${ev.category}</strong></div>`;

            const seatsFull = (ev.seatsLeft <= 0);

            cardCol.innerHTML = `
                <div class="card h-100 event-card shadow-sm">
                    <div class="card-body p-4 d-flex flex-column">
                        <div class="d-flex justify-content-between align-items-start mb-2">
                            <span class="badge ${typeBadgeClass} px-2 py-1">${ev.type}</span>
                            <span class="badge bg-light text-secondary border">${ev.clubName}</span>
                        </div>
                        <h5 class="card-title fw-bold text-dark mb-2">${ev.name}</h5>
                        <p class="card-text text-muted small flex-grow-1">${ev.description}</p>
                        ${extraInfo}
                        <div class="border-top my-3 pt-2 text-muted small">
                            <div class="d-flex justify-content-between mb-1">
                                <span><i class="bi bi-calendar3 me-1"></i> ${ev.date}</span>
                                <span><i class="bi bi-clock me-1"></i> ${ev.time}</span>
                            </div>
                            <div><i class="bi bi-geo-alt me-1"></i> ${ev.venue}</div>
                        </div>

                        <!-- Seats Progress -->
                        <div class="mb-3">
                            <div class="d-flex justify-content-between small text-muted mb-1">
                                <span>Seats Remaining</span>
                                <span class="${seatsFull ? 'text-danger fw-bold' : 'fw-semibold'}">${ev.seatsLeft} / ${ev.totalSeats}</span>
                            </div>
                            <div class="progress" style="height: 6px;">
                                <div class="progress-bar ${seatsFull ? 'bg-danger' : 'bg-primary'}" 
                                     style="width: ${(ev.seatsLeft / ev.totalSeats) * 100}%"></div>
                            </div>
                        </div>

                        <!-- Fee & Action -->
                        <div class="d-flex justify-content-between align-items-center mt-auto pt-2 border-top">
                            <div>
                                <span class="text-muted small d-block">Registration</span>
                                <span class="fee-display">${baseFee === 0 ? 'FREE' : `INR ${payableFee.toFixed(2)}`}</span>
                                ${isMember && baseFee > 0 ? '<span class="badge bg-success-subtle text-success border border-success-subtle ms-1" style="font-size: 0.65rem;">20% Member Off</span>' : ''}
                            </div>
                            <button class="btn ${seatsFull ? 'btn-secondary disabled' : 'btn-primary'} px-3 fw-semibold btn-enroll" 
                                    data-event-id="${ev.eventId}" ${seatsFull ? 'disabled' : ''}>
                                ${seatsFull ? 'Full' : 'Enroll Now'}
                            </button>
                        </div>
                    </div>
                </div>
            `;

            grid.appendChild(cardCol);
        });

        // Attach enroll button listeners
        document.querySelectorAll('.btn-enroll').forEach(b => {
            b.addEventListener('click', (e) => {
                const eventId = parseInt(b.dataset.eventId);
                const ev = currentEvents.find(x => x.eventId === eventId);
                if (ev) openEnrollModal(ev);
            });
        });
    }

    // Filter Listeners
    ['filterName', 'filterClub', 'filterMaxFee', 'filterSort'].forEach(id => {
        document.getElementById(id).addEventListener('input', loadEvents);
    });
    document.getElementById('btnResetFilters').addEventListener('click', () => {
        document.getElementById('filterName').value = '';
        document.getElementById('filterClub').value = '';
        document.getElementById('filterMaxFee').value = '';
        document.getElementById('filterSort').value = 'date';
        loadEvents();
    });

    // 2. Open Confirmation Modal Before Payment
    function openEnrollModal(ev) {
        pendingEnrollmentEvent = ev;
        document.getElementById('confirmEventName').textContent = ev.name;
        document.getElementById('confirmEventClub').textContent = ev.clubName;
        document.getElementById('confirmEventDate').textContent = ev.date;
        document.getElementById('confirmEventTime').textContent = ev.time;
        document.getElementById('confirmEventVenue').textContent = ev.venue;

        const baseFee = parseFloat(ev.fee);
        const payableFee = (user.membership === 'member') ? (baseFee * 0.80) : baseFee;
        document.getElementById('confirmEventFee').textContent = (baseFee === 0) ? 'FREE' : `INR ${payableFee.toFixed(2)}`;

        enrollConfirmModal.show();
    }

    // 3. User Clicks "Proceed" in Confirmation Modal
    document.getElementById('btnProceedEnroll').addEventListener('click', async () => {
        if (!pendingEnrollmentEvent) return;
        enrollConfirmModal.hide();

        try {
            const enrollment = await API.enroll(pendingEnrollmentEvent.eventId);
            activeEnrollmentId = enrollment.enrollmentId;

            // Refresh events to reflect decremented seat
            loadEvents();

            const baseFee = parseFloat(pendingEnrollmentEvent.fee);
            const payableFee = (user.membership === 'member') ? (baseFee * 0.80) : baseFee;

            if (payableFee <= 0.001) {
                // Free event: automatically initiates payment & generates free receipt
                const pay = await API.initiatePayment(activeEnrollmentId, 'Cash', {});
                const myEnrollments = await API.getMyEnrollments();
                const myEn = myEnrollments.find(x => x.enrollmentId === activeEnrollmentId);
                if (myEn && myEn.receiptNo) {
                    const rcpt = await API.getReceipt(myEn.receiptNo);
                    displayReceipt(rcpt);
                } else {
                    showPageAlert('Successfully enrolled in free event!', 'success');
                }
            } else {
                // Paid event: open Payment Modal
                document.getElementById('payModalAmount').textContent = `INR ${payableFee.toFixed(2)}`;
                document.getElementById('payModalStudentType').textContent = (user.membership === 'member') ? 'Club Member (20% Discount Applied)' : 'Guest Student';
                document.getElementById('upiStep1').classList.remove('d-none');
                document.getElementById('upiStep2').classList.add('d-none');
                document.getElementById('payAlert').classList.add('d-none');
                paymentModal.show();
            }
        } catch (err) {
            let friendly = err.message;
            if (err.status === 409) {
                if (friendly.includes('full')) friendly = 'Sorry, this event is full. No seats remaining.';
                else if (friendly.includes('already enrolled')) friendly = 'You are already enrolled in this event.';
                else if (friendly.includes('clash') || friendly.includes('Clash')) friendly = 'Schedule conflict: Time clash with another event already in your schedule.';
            }
            showPageAlert(friendly, 'danger');
        }
    });

    // 4. Payment Workflows
    // UPI Step 1 -> Initiate
    document.getElementById('btnInitiateUpi').addEventListener('click', async () => {
        try {
            document.getElementById('payAlert').classList.add('d-none');
            const studentUpi = document.getElementById('upiStudentId').value.trim() || 'student@upi';
            const payRes = await API.initiatePayment(activeEnrollmentId, 'UPI', { studentUpiId: studentUpi });
            activePaymentId = payRes.paymentId;

            // Render Dynamic UPI QR Code
            const qrEl = document.getElementById('qrcode');
            qrEl.innerHTML = '';
            new QRCode(qrEl, {
                text: payRes.instruction,
                width: 180,
                height: 180,
                colorDark: '#0f172a',
                colorLight: '#ffffff',
                correctLevel: QRCode.CorrectLevel.M
            });

            document.getElementById('upiDeepLinkText').textContent = payRes.instruction;
            document.getElementById('upiStep1').classList.add('d-none');
            document.getElementById('upiStep2').classList.remove('d-none');
        } catch (err) {
            document.getElementById('payAlert').textContent = err.message;
            document.getElementById('payAlert').classList.remove('d-none');
        }
    });

    // UPI Step 2 -> Confirm (Simulated)
    document.getElementById('btnConfirmUpi').addEventListener('click', async () => {
        try {
            const conf = await API.confirmPayment(activePaymentId);
            paymentModal.hide();
            displayReceipt(conf.receipt);
        } catch (err) {
            document.getElementById('payAlert').textContent = err.message;
            document.getElementById('payAlert').classList.remove('d-none');
        }
    });

    // Card Payment Workflow
    document.getElementById('btnPayCard').addEventListener('click', async () => {
        try {
            document.getElementById('payAlert').classList.add('d-none');
            const cardNum = document.getElementById('cardNum').value.trim();
            const holder = document.getElementById('cardHolder').value.trim() || user.name;

            const payRes = await API.initiatePayment(activeEnrollmentId, 'Card', { cardNumber: cardNum, cardHolderName: holder });
            activePaymentId = payRes.paymentId;

            const conf = await API.confirmPayment(activePaymentId);
            paymentModal.hide();
            displayReceipt(conf.receipt);
        } catch (err) {
            let msg = err.message;
            if (err.status === 402) {
                msg = 'Card Payment Failed: Card number must be exactly 16 digits.';
            }
            document.getElementById('payAlert').textContent = msg;
            document.getElementById('payAlert').classList.remove('d-none');
        }
    });

    // Cash Payment Workflow
    document.getElementById('btnPayCash').addEventListener('click', async () => {
        try {
            document.getElementById('payAlert').classList.add('d-none');
            const payRes = await API.initiatePayment(activeEnrollmentId, 'Cash', {});
            activePaymentId = payRes.paymentId;

            const conf = await API.confirmPayment(activePaymentId);
            paymentModal.hide();
            displayReceipt(conf.receipt);
        } catch (err) {
            document.getElementById('payAlert').textContent = err.message;
            document.getElementById('payAlert').classList.remove('d-none');
        }
    });

    // 5. Display Official Receipt with QR Code
    function displayReceipt(receipt) {
        document.getElementById('rcptCode').textContent = receipt.receiptCode;
        document.getElementById('rcptDate').textContent = receipt.timestamp;
        document.getElementById('rcptStudentId').textContent = '#' + receipt.studentId;
        document.getElementById('rcptEnrollId').textContent = '#' + receipt.enrollmentId;
        document.getElementById('rcptTxnId').textContent = receipt.transactionId;
        document.getElementById('rcptMethod').textContent = receipt.paymentMethod;
        document.getElementById('rcptAmount').textContent = `INR ${parseFloat(receipt.amountPaid).toFixed(2)}`;

        // Render QR Code encoding "CC-RCPT-<number>"
        const rcptQrEl = document.getElementById('receiptQrCode');
        rcptQrEl.innerHTML = '';
        new QRCode(rcptQrEl, {
            text: receipt.receiptCode,
            width: 140,
            height: 140,
            colorDark: '#0f172a',
            colorLight: '#ffffff',
            correctLevel: QRCode.CorrectLevel.M
        });
        document.getElementById('receiptQrText').textContent = receipt.receiptCode;

        receiptModal.show();
    }

    // 6. My Enrollments Modal View
    document.getElementById('btnMyEnrollments').addEventListener('click', async () => {
        try {
            const enrollments = await API.getMyEnrollments();
            const allEvents = await API.getEvents({});
            const listEl = document.getElementById('myEnrollmentsList');
            listEl.innerHTML = '';

            if (enrollments.length === 0) {
                listEl.innerHTML = `<tr><td colspan="5" class="text-center text-muted py-4">No active enrollments found.</td></tr>`;
            } else {
                enrollments.forEach(en => {
                    const ev = allEvents.find(x => x.eventId === en.eventId) || { name: `Event #${en.eventId}` };
                    const tr = document.createElement('tr');
                    tr.innerHTML = `
                        <td class="font-monospace fw-semibold">#${en.enrollmentId}</td>
                        <td><strong>${ev.name}</strong></td>
                        <td class="small text-muted">${en.enrollmentDate}</td>
                        <td>
                            <span class="badge ${en.attended ? 'bg-success' : 'bg-secondary'}">
                                ${en.attended ? 'Present' : 'Not Marked'}
                            </span>
                        </td>
                        <td>
                            <div class="btn-group btn-group-sm">
                                ${en.hasReceipt ? `<button class="btn btn-outline-primary btn-view-rcpt" data-rcpt="${en.receiptNo}">Receipt</button>` : ''}
                                <button class="btn btn-outline-success btn-cert" data-eid="${en.enrollmentId}">Certificate</button>
                            </div>
                        </td>
                    `;
                    listEl.appendChild(tr);
                });

                // Attach button actions
                document.querySelectorAll('.btn-view-rcpt').forEach(b => {
                    b.addEventListener('click', async () => {
                        const rId = b.dataset.rcpt;
                        const rcpt = await API.getReceipt(rId);
                        myEnrollmentsModal.hide();
                        displayReceipt(rcpt);
                    });
                });

                document.querySelectorAll('.btn-cert').forEach(b => {
                    b.addEventListener('click', async () => {
                        const enId = b.dataset.eid;
                        try {
                            const res = await API.getCertificate(enId);
                            document.getElementById('certificateContent').textContent = res.certificate;
                            myEnrollmentsModal.hide();
                            certificateModal.show();
                        } catch (err) {
                            alert(err.message || 'Certificate cannot be generated.');
                        }
                    });
                });
            }

            myEnrollmentsModal.show();
        } catch (err) {
            showPageAlert('Failed to load enrollments: ' + err.message);
        }
    });

    // Initial load
    loadEvents();
});
