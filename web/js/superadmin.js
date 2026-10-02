// SuperAdmin Portal Page Logic

document.addEventListener('DOMContentLoaded', () => {
    const user = API.getUser();
    const token = API.getToken();
    if (!user || !token || user.role !== 'SuperAdmin') {
        window.location.href = 'index.html';
        return;
    }

    // Set navbar
    document.getElementById('navSuperAdminName').textContent = user.name;
    document.getElementById('btnLogout').addEventListener('click', () => API.logout());

    function showAlert(msg, isSuccess = false) {
        const el = document.getElementById('superAlert');
        const msgEl = document.getElementById('superAlertMsg');
        el.className = `alert alert-${isSuccess ? 'success' : 'danger'} alert-dismissible fade show`;
        msgEl.textContent = msg;
        el.classList.remove('d-none');
        window.scrollTo({ top: 0, behavior: 'smooth' });
    }

    // 1. Load Pending Events
    async function loadPending() {
        try {
            const pendingEvents = await API.getPendingEvents();
            document.getElementById('statPending').textContent = pendingEvents.length;

            const tbody = document.getElementById('pendingTableBody');
            tbody.innerHTML = '';

            if (pendingEvents.length === 0) {
                tbody.innerHTML = `<tr><td colspan="5" class="text-center text-muted py-4"><i class="bi bi-check2-circle fs-4 d-block mb-1 text-success"></i> No events currently pending review. All caught up!</td></tr>`;
                return;
            }

            pendingEvents.forEach(ev => {
                let badgeClass = 'badge-workshop';
                if (ev.type === 'Hackathon') badgeClass = 'badge-hackathon';
                else if (ev.type === 'Seminar') badgeClass = 'badge-seminar';
                else if (ev.type === 'CulturalEvent') badgeClass = 'badge-cultural';

                const tr = document.createElement('tr');
                tr.innerHTML = `
                    <td>
                        <span class="badge ${badgeClass} mb-1">${ev.type}</span>
                        <strong class="d-block text-dark">${ev.name}</strong>
                        <div class="text-muted small">${ev.description}</div>
                    </td>
                    <td><span class="badge bg-light text-dark border">${ev.clubName}</span></td>
                    <td class="small text-muted">
                        <div><i class="bi bi-calendar3 me-1"></i> ${ev.date} ${ev.time}</div>
                        <div><i class="bi bi-geo-alt me-1"></i> ${ev.venue}</div>
                    </td>
                    <td>
                        <div class="fw-semibold">INR ${parseFloat(ev.fee).toFixed(2)}</div>
                        <div class="small text-muted">${ev.totalSeats} seats</div>
                    </td>
                    <td class="text-end">
                        <div class="btn-group btn-group-sm">
                            <button class="btn btn-success fw-medium btn-approve" data-id="${ev.eventId}">
                                <i class="bi bi-check-lg"></i> Approve
                            </button>
                            <button class="btn btn-outline-danger btn-reject" data-id="${ev.eventId}">
                                <i class="bi bi-x-lg"></i> Reject
                            </button>
                        </div>
                    </td>
                `;
                tbody.appendChild(tr);
            });

            // Action listeners
            document.querySelectorAll('.btn-approve').forEach(btn => {
                btn.addEventListener('click', async () => {
                    const id = btn.dataset.id;
                    try {
                        await API.approveEvent(id);
                        showAlert(`Event #${id} approved successfully! Now live on portal.`, true);
                        refreshAll();
                    } catch (err) {
                        showAlert('Failed to approve event: ' + err.message);
                    }
                });
            });

            document.querySelectorAll('.btn-reject').forEach(btn => {
                btn.addEventListener('click', async () => {
                    const id = btn.dataset.id;
                    try {
                        await API.rejectEvent(id);
                        showAlert(`Event #${id} has been rejected.`, true);
                        refreshAll();
                    } catch (err) {
                        showAlert('Failed to reject event: ' + err.message);
                    }
                });
            });
        } catch (err) {
            showAlert('Failed to load pending events: ' + err.message);
        }
    }

    // 2. Load Reports & Analytics
    async function loadReports() {
        try {
            const reports = await API.getReports();
            const tbody = document.getElementById('reportsTableBody');
            tbody.innerHTML = '';

            let totalRev = 0;
            let totalEnroll = 0;

            reports.forEach(r => {
                totalRev += r.revenue;
                totalEnroll += r.seatsEnrolled;

                let statusBadge = '<span class="badge bg-success">APPROVED</span>';
                if (r.status === 'PENDING') statusBadge = '<span class="badge bg-warning text-dark">PENDING</span>';
                else if (r.status === 'REJECTED') statusBadge = '<span class="badge bg-danger">REJECTED</span>';

                const tr = document.createElement('tr');
                tr.innerHTML = `
                    <td class="font-monospace text-muted">#${r.eventId}</td>
                    <td><strong>${r.name}</strong></td>
                    <td><span class="badge bg-light text-dark border">${r.clubName}</span></td>
                    <td>${statusBadge}</td>
                    <td>${r.seatsEnrolled} / ${r.totalSeats}</td>
                    <td>
                        <span class="${r.seatsLeft === 0 ? 'text-danger fw-bold' : ''}">${r.seatsLeft}</span>
                    </td>
                    <td class="text-end fw-bold text-dark">INR ${parseFloat(r.revenue).toFixed(2)}</td>
                `;
                tbody.appendChild(tr);
            });

            document.getElementById('statRevenue').textContent = `INR ${totalRev.toFixed(2)}`;
            document.getElementById('statEnrollments').textContent = totalEnroll;
        } catch (err) {
            showAlert('Failed to load analytics: ' + err.message);
        }
    }

    function refreshAll() {
        loadPending();
        loadReports();
    }

    document.getElementById('btnRefreshPending').addEventListener('click', loadPending);
    document.getElementById('btnRefreshReports').addEventListener('click', loadReports);

    // Initial load
    refreshAll();
});
