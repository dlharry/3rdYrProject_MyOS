import matplotlib.pyplot as plt
import matplotlib.dates as mdates
from datetime import datetime

# Given commit dates and titles from the image
commits_info = [
    ('Apr 11, 2024', 'update report'),
    ('Mar 14, 2024', 'Update README.md'),
    ('Mar 14, 2024', 'fix bug'),
    ('Mar 13, 2024', 'problem found, the drawingString function will make laggy'),
    ('Mar 13, 2024', 're-enabled mmu'),
    ('Mar 13, 2024', 'Road assert'),
    ('Mar 13, 2024', 'breakout main loop'),
    ('Mar 12, 2024', 'remove mmu, rewrite fb, breakout but irq interrupting game'),
    ('Mar 12, 2024', 'remove useless bot.S'),
    ('Mar 10, 2024', 'mmu enable, paging'),
    ('Mar 9, 2024', 'a little test on exceptin(data abort)'),
    ('Mar 9, 2024', 'uart instead of mini uart & assert'),
    ('Mar 8, 2024', 'remove old kernel.c'),
    ('Mar 8, 2024', 'Merge branch \'main\' of github.com:dlharry/3rdYrProject_MyOS'),
    ('Mar 8, 2024', 'Update README.md'),
    ('Mar 7, 2024', 'dma enable and test against normal'),
    ('Mar 7, 2024', 'hdmi video mailbox'),
    ('Mar 7, 2024', 'some status reader by mailbox'),
    ('Mar 6, 2024', 'mailbox but no bool'),
    ('Mar 6, 2024', 'sleep function'),
    ('Feb 29, 2024', 'timer'),
    ('Feb 27, 2024', 'update timer but not working'),
    ('Feb 27, 2024', 'delete old files'),
    ('Feb 27, 2024', 'debug uart'),
    ('Feb 27, 2024', 'irq handle'),
    ('Feb 27, 2024', 'now in el1, eret miss'),
    ('Feb 27, 2024', 'setting el1 but appears to be 3'),
    ('Feb 27, 2024', 'switch to el3 but switched to el1'),
    ('Feb 27, 2024', 'print exception level'),
    ('Feb 27, 2024', 'support printf')
]

# Parse the dates and sort the commits by date
commits_info = [(datetime.strptime(date, '%b %d, %Y'), title) for date, title in commits_info]
commits_info.sort(key=lambda x: x[0])

# Create lists of start dates and end dates for the Gantt bars, assuming each task takes a day
dates = [info[0] for info in commits_info]
start_dates = dates[:-1]  # Start date is the day of the previous commit
end_dates = dates[1:]     # End date is the day of the current commit
titles = [info[1] for info in commits_info][1:]  # Task names, omitting the first since it has no start date

# Create the Gantt chart
fig, ax = plt.subplots(figsize=(20, 5))

# Create a barh for each commit
for i, (start, end, title) in enumerate(zip(start_dates, end_dates, titles)):
    ax.barh(y=i, left=start, width=(end - start).days, height=0.4, align='center')

# Set the yticks to be the commit titles
ax.set(yticks=range(len(titles)), yticklabels=titles)

# Format the x-axis to show the date
plt.gca().xaxis.set_major_formatter(mdates.DateFormatter('%b %d, %Y'))
plt.gca().xaxis.set_major_locator(mdates.DayLocator())

# Improve layout
plt.gcf().autofmt_xdate()  # Rotate the x-axis labels to fit
plt.tight_layout()

# Set labels and title
ax.set_xlabel('Date')
ax.set_ylabel('Commit Task')
ax.set_title('Project Commit History Gantt Chart')

# Show the Gantt chart
plt.show()