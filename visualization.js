document.addEventListener('DOMContentLoaded', () => {
    const chartContainer = document.getElementById('chart-container');

    // Sample data from the C++ program's output
    const processes = ['P1', 'P2', 'P3'];
    const timeline = [
        ['*', ' ', ' '],
        ['*', '*', ' '],
        [' ', '*', '*'],
        [' ', ' ', '*']
    ];

    processes.forEach((process, index) => {
        const processRow = document.createElement('div');
        processRow.classList.add('process-row');

        const processLabel = document.createElement('div');
        processLabel.classList.add('process-label');
        processLabel.textContent = process;
        processRow.appendChild(processLabel);

        timeline.forEach((timeSlot, timeIndex) => {
            const timeSlotDiv = document.createElement('div');
            timeSlotDiv.classList.add('time-slot');
            if (timeSlot[index] === '*') {
                timeSlotDiv.classList.add('active');
            }
            processRow.appendChild(timeSlotDiv);
        });

        chartContainer.appendChild(processRow);
    });
});
