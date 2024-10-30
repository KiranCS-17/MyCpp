import os
import json
import time
from inotify_simple import INotify, flags

json_file_path = "/home/kics/config.json"
watched_paths = ["key1", "nested.key2"]
last_config = None
last_mtime = None

# Helper functions for loading JSON and checking paths
def load_json(file_path):
    with open(file_path, "r") as f:
        return json.load(f)

def get_value_from_path(data, path):
    keys = path.split('.')
    for key in keys:
        data = data.get(key)
        if data is None:
            break
    return data

def check_for_changes(current_config):
    global last_config
    if last_config is None:
        last_config = current_config
        return []
    
    changes = []
    for path in watched_paths:
        old_value = get_value_from_path(last_config, path)
        new_value = get_value_from_path(current_config, path)
        if old_value != new_value:
            changes.append((path, old_value, new_value))
    last_config = current_config
    return changes

# Initialize inotify and start watching
inotify = INotify()
wd = inotify.add_watch(json_file_path, flags.MODIFY)
print(f"Watching {json_file_path} for specific path changes...")

try:
    while True:
        # Polling fallback: Check file modification time every second
        current_mtime = os.path.getmtime(json_file_path)
        if last_mtime is None or current_mtime != last_mtime:
            last_mtime = current_mtime
            current_config = load_json(json_file_path)
            changes = check_for_changes(current_config)
            if changes:
                for path, old, new in changes:
                    print(f"Change detected in path '{path}': {old} -> {new}")

        # Check for inotify events (non-blocking)
        events = inotify.read(timeout=1000)
        for event in events:
            if event.mask & flags.MODIFY:
                current_config = load_json(json_file_path)
                changes = check_for_changes(current_config)
                if changes:
                    for path, old, new in changes:
                        print(f"Change detected in path '{path}': {old} -> {new}")
        
        time.sleep(1)  # Poll every second if no inotify events
except KeyboardInterrupt:
    print("Stopping inotify watcher.")
finally:
    inotify.rm_watch(wd)

