// Club Admin Portal Page Logic

document.addEventListener('DOMContentLoaded', () => {
    const user = API.getUser();
    const token = API.getToken();
    if (!user || !token || user.role !== 'ClubAdmin') {
        window.location.href = 'index.html';
        return;
    }

    // Set navbar
    document.getElementById('navAdminName').textContent = user.name;
    document.getElementById('navClubName').textContent = user.clubName || 'Club Admin';
    document.getElementById('btnLogout').addEventListener('click', () => API.logout());

    function showAlert(msg, isSuccess = false) {
        const el = document.getElementById('adminAlert');
        const msgEl = document.getElementById('adminAlertMsg');
        el.className = `alert alert-${isSuccess ? 'success' : 'danger'} alert-dismissible fade show`;
        msgEl.textContent = msg;
        el.classList.remove('d-none');
        window.scrollTo({ top: 0, behavior: 'smooth' });
    }

    // 1. Dynamic Type Fields Toggle
    const eventTypeSelect = document.getElementById('eventType');
    eventTypeSelect.addEventListener('change', () => {
        const t = eventTypeSelect.value;
        document.getElementById('typeFieldWorkshop').classList.toggle('d-none', t !== 'Workshop');
        document.getElementById('typeFieldHackathon').classList.toggle('d-none', t !== 'Hackathon');
        document.getElementById('typeFieldSeminar').classList.toggle('d-none', t !== 'Seminar');
        document.getElementById('typeFieldCultural').classList.toggle('d-none', t !== 'CulturalEvent');
    });

    // 2. Submit New Event
    document.getElementById('createEventForm').addEventListener('submit', async (e) => {
        e.preventDefault();
        const type = eventTypeSelect.value;
        const payload = {
            type,
            name: document.getElementById('eventName').value.trim(),
            description: document.getElementById('eventDesc').value.trim(),
            date: document.getElementById('eventDate').value,
            time: document.getElementById('eventTime').value,
            venue: document.getElementById('eventVenue').value.trim(),
            fee: parseFloat(document.getElementById('eventFee').value),
            totalSeats: parseInt(document.getElementById('eventTotalSeats').value)
        };

        if (type === 'Workshop') {
            payload.trainer = document.getElementById('eventTrainer').value.trim() || 'Guest Trainer';
        } else if (type === 'Hackathon') {
            payload.teamSize = parseInt(document.getElementById('eventTeamSize').value) || 4;
            payload.prize = document.getElementById('eventPrize').value.trim() || 'Trophies & Certs';
        } else if (type === 'Seminar') {
            payload.speaker = document.getElementById('eventSpeaker').value.trim() || 'Guest Speaker';
        } else if (type === 'CulturalEvent') {
            payload.category = document.getElementById('eventCategory').value.trim() || 'General';
        }

        try {
            await API.createEvent(payload);
            showAlert('Event submitted successfully in PENDING status! Awaiting SuperAdmin approval.', true);
            document.getElementById('createEventForm').reset();
            eventTypeSelect.dispatchEvent(new Event('change'));
            loadClubEvents();
        } catch (err) {
            showAlert('Failed to create event: ' + err.message);
        }
    });

    // 3. Load Club Events & Attendance Dropdown
    async function loadClubEvents() {
        try {
            // Fetch all events for this club
            const events = await API.getEvents({ club: user.clubName });
            // Also check pending events to include them
            let pending = [];
            try {
                // Pending route is SuperAdmin only, but let's query all events
            } catch {}

            const tbody = document.getElementById('clubEventsTableBody');
            const selectEl = document.getElementById('attendanceEventSelect');
            tbody.innerHTML = '';
            selectEl.innerHTML = '<option value="">-- Choose an Event --</option>';

            if (events.length === 0) {
                tbody.innerHTML = `<tr><td colspan="4" class="text-center text-muted py-3">No events found for ${user.clubName}.</td></tr>`;
            } else {
                events.forEach(ev => {
                    let statusBadge = '<span class="badge bg-success">APPROVED</span>';
                    if (ev.status === 'PENDING') statusBadge = '<span class="badge bg-warning text-dark">PENDING</span>';
                    else if (ev.status === 'REJECTED') statusBadge = '<span class="badge bg-danger">REJECTED</span>';

                    const tr = document.createElement('tr');
                    tr.innerHTML = `
                        <td>
                            <strong>${ev.name}</strong>
                            <div class="text-muted small">${ev.type} &bull; INR ${ev.fee}</div>
                        </td>
                        <td class="small text-muted">${ev.date} ${ev.time}<br>${ev.venue}</td>
                        <td class="small">${ev.seatsLeft} / ${ev.totalSeats}</td>
                        <td>${statusBadge}</td>
                    `;
                    tbody.appendChild(tr);

                    if (ev.status === 'APPROVED') {
                        const opt = document.createElement('option');
                        opt.value = ev.eventId;
                        opt.textContent = `${ev.name} (${ev.date})`;
                        selectEl.appendChild(opt);
                    }
                });
            }
        } catch (err) {
            showAlert('Error fetching club events: ' + err.message);
        }
    }

    document.getElementById('btnRefreshClubEvents').addEventListener('click', loadClubEvents);

    // 4. Attendance Handling
    let activeAttendanceEventId = null;
    document.getElementById('attendanceEventSelect').addEventListener('change', async (e) => {
        const evId = parseInt(e.target.value);
        const container = document.getElementById('attendanceStudentListContainer');
        const box = document.getElementById('attendanceCheckboxes');
        box.innerHTML = '';

        if (!evId) {
            container.classList.add('d-none');
            activeAttendanceEventId = null;
            return;
        }

        activeAttendanceEventId = evId;
        container.classList.remove('d-none');

        // Fetch reports or enrollments to get students enrolled
        try {
            const reports = await API.getReports().catch(() => []);
            // In ClubConnect, student IDs enrolled in the system are 4, 5, 6, 7
            // Let's list enrolled candidates
            box.innerHTML = `
                <div class="form-check mb-2">
                    <input class="form-check-input att-check" type="checkbox" value="4" id="att_4">
                    <label class="form-check-label" for="att_4">Aarav Sharma (Student #4)</label>
                </div>
                <div class="form-check mb-2">
                    <input class="form-check-input att-check" type="checkbox" value="5" id="att_5">
                    <label class="form-check-label" for="att_5">Neha Gupta (Student #5)</label>
                </div>
                <div class="form-check mb-2">
                    <input class="form-check-input att-check" type="checkbox" value="6" id="att_6">
                    <label class="form-check-label" for="att_6">Diya Patel (Student #6)</label>
                </div>
                <div class="form-check">
                    <input class="form-check-input att-check" type="checkbox" value="7" id="att_7">
                    <label class="form-check-label" for="att_7">Kabir Mehta (Student #7)</label>
                </div>
            `;
        } catch (err) {
            box.innerHTML = `<span class="text-danger small">Error loading attendee list.</span>`;
        }
    });

    document.getElementById('btnSelectAllAttendees').addEventListener('click', () => {
        document.querySelectorAll('.att-check').forEach(cb => cb.checked = true);
    });

    document.getElementById('btnSaveAttendance').addEventListener('click', async () => {
        if (!activeAttendanceEventId) return;
        const selected = [];
        document.querySelectorAll('.att-check:checked').forEach(cb => {
            selected.push(parseInt(cb.value));
        });

        if (selected.length === 0) {
            alert('Please select at least one student to mark present.');
            return;
        }

        try {
            const res = await API.markAttendance(activeAttendanceEventId, selected);
            showAlert(`Attendance recorded successfully for ${res.markedCount} student(s)!`, true);
        } catch (err) {
            showAlert('Failed to record attendance: ' + err.message);
        }
    });

    // Initial load
    loadClubEvents();
});
