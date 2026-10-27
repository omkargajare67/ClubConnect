// Club Admin Portal Page Logic

document.addEventListener('DOMContentLoaded', () => {
    const user = API.getUser();
    const token = API.getToken();
    if (!user || !token || user.role !== 'ClubAdmin') {
        window.location.href = 'index.html';
        return;
    }

    // Set navbar & hero banner
    document.getElementById('navAdminName').textContent = user.name;
    document.getElementById('navClubName').textContent = user.clubName || 'Club Admin';
    const heroAdminName = document.getElementById('heroAdminName');
    if (heroAdminName) heroAdminName.textContent = user.name;
    const heroAdminClubTitle = document.getElementById('heroAdminClubTitle');
    if (heroAdminClubTitle) heroAdminClubTitle.textContent = `${user.clubName || 'Club'} Dashboard`;
    const heroAdminClubBadge = document.getElementById('heroAdminClubBadge');
    if (heroAdminClubBadge) heroAdminClubBadge.textContent = user.clubName || 'Club Admin';
    const attendanceScopeName = document.getElementById('attendanceClubScopeName');
    if (attendanceScopeName) attendanceScopeName.textContent = user.clubName || 'Your Club';
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

    async function loadAttendeesForEvent(evId) {
        const container = document.getElementById('attendanceStudentListContainer');
        const box = document.getElementById('attendanceCheckboxes');
        box.innerHTML = '<div class="text-center text-muted py-3"><div class="spinner-border spinner-border-sm me-2" role="status"></div>Loading real enrolled students...</div>';
        container.classList.remove('d-none');
        activeAttendanceEventId = evId;

        try {
            const attendees = await API.getAttendees(evId);
            box.innerHTML = '';

            if (!attendees || attendees.length === 0) {
                box.innerHTML = `
                    <div class="text-center text-muted py-3">
                        <i class="bi bi-person-x fs-3 d-block mb-1 text-secondary"></i>
                        No students have enrolled in this event yet.
                    </div>
                `;
                document.getElementById('btnSaveAttendance').disabled = true;
                return;
            }

            document.getElementById('btnSaveAttendance').disabled = false;
            attendees.forEach(att => {
                const itemDiv = document.createElement('div');
                itemDiv.className = 'form-check d-flex align-items-center justify-content-between py-2 border-bottom border-light-subtle';
                
                const leftDiv = document.createElement('div');
                leftDiv.className = 'd-flex align-items-center';

                const input = document.createElement('input');
                input.className = 'form-check-input att-check me-2 mt-0';
                input.type = 'checkbox';
                input.value = att.studentId;
                input.id = `att_${att.studentId}`;
                input.checked = !!att.attended;

                const label = document.createElement('label');
                label.className = 'form-check-label';
                label.htmlFor = `att_${att.studentId}`;
                
                const memBadge = att.membership === 'member'
                    ? '<span class="badge bg-success-subtle text-success border border-success-subtle ms-1" style="font-size: 0.7rem;">Member</span>'
                    : '<span class="badge bg-secondary-subtle text-secondary border border-secondary-subtle ms-1" style="font-size: 0.7rem;">Guest</span>';

                label.innerHTML = `
                    <strong>${att.studentName}</strong> ${memBadge}
                    <div class="text-muted small">
                        Roll: <code>${att.rollNo || 'N/A'}</code> &bull; Dept: ${att.department || '-'} &bull; ${att.studentEmail}
                    </div>
                `;

                leftDiv.appendChild(input);
                leftDiv.appendChild(label);

                const statusSpan = document.createElement('span');
                statusSpan.id = `status_badge_${att.studentId}`;
                if (att.attended) {
                    statusSpan.className = 'badge bg-success';
                    statusSpan.innerHTML = '<i class="bi bi-check-lg me-1"></i>Present';
                } else {
                    statusSpan.className = 'badge bg-light text-muted border';
                    statusSpan.textContent = 'Absent';
                }

                input.addEventListener('change', () => {
                    if (input.checked) {
                        statusSpan.className = 'badge bg-success';
                        statusSpan.innerHTML = '<i class="bi bi-check-lg me-1"></i>Present';
                    } else {
                        statusSpan.className = 'badge bg-light text-muted border';
                        statusSpan.textContent = 'Absent';
                    }
                });

                itemDiv.appendChild(leftDiv);
                itemDiv.appendChild(statusSpan);
                box.appendChild(itemDiv);
            });
        } catch (err) {
            box.innerHTML = `<div class="alert alert-danger py-2 mb-0 small">Failed to load enrolled students: ${err.message}</div>`;
        }
    }

    document.getElementById('attendanceEventSelect').addEventListener('change', async (e) => {
        const evId = parseInt(e.target.value);
        if (!evId) {
            document.getElementById('attendanceStudentListContainer').classList.add('d-none');
            activeAttendanceEventId = null;
            return;
        }
        await loadAttendeesForEvent(evId);
    });

    document.getElementById('btnSelectAllAttendees').addEventListener('click', () => {
        document.querySelectorAll('.att-check').forEach(cb => {
            cb.checked = true;
            cb.dispatchEvent(new Event('change'));
        });
    });

    const deselectBtn = document.getElementById('btnDeselectAllAttendees');
    if (deselectBtn) {
        deselectBtn.addEventListener('click', () => {
            document.querySelectorAll('.att-check').forEach(cb => {
                cb.checked = false;
                cb.dispatchEvent(new Event('change'));
            });
        });
    }

    document.getElementById('btnSaveAttendance').addEventListener('click', async () => {
        if (!activeAttendanceEventId) return;
        const selected = [];
        document.querySelectorAll('.att-check:checked').forEach(cb => {
            selected.push(parseInt(cb.value));
        });

        try {
            const res = await API.markAttendance(activeAttendanceEventId, selected);
            showAlert(`Attendance recorded successfully for ${res.markedCount} present student(s)!`, true);
            await loadAttendeesForEvent(activeAttendanceEventId);
        } catch (err) {
            showAlert('Failed to record attendance: ' + err.message);
        }
    });

    // Initial load
    loadClubEvents();
});
