"""Collect existing optimization captures without running or changing the demo."""
from __future__ import print_function
import json
import os
from PIL import Image, ImageChops, ImageDraw

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
CAPTURES = os.path.join(ROOT, 'gbDemo', 'output', 'island-check')


def read_json(path):
    with open(path, 'rb') as source:
        return json.load(source)


def capture(label, accepted, purpose):
    folder = os.path.join(CAPTURES, 'perf-' + label + '-200')
    result = {'label': label, 'accepted': accepted, 'purpose': purpose,
              'summary': read_json(os.path.join(folder, 'summary.json'))}
    details = os.path.join(folder, 'profile-details.json')
    if os.path.isfile(details):
        profile = read_json(details)
        result['profile'] = {key: profile.get(key) for key in
                             ('frame_ms', 'gpu_frame_ms', 'draw_calls',
                              'triangles', 'upload_call_ms', 'upload_frame_ms')}
        for domain in ('cpu', 'gpu'):
            result['profile'][domain] = {
                key: {field: value.get(field) for field in ('mean_ms', 'mean_calls')}
                for key, value in profile[domain].items()}
    errors = os.path.join(folder, 'errors.log')
    result['stderr_bytes'] = os.path.getsize(errors)
    return result


def quality(label):
    path = os.path.join(CAPTURES, 'quality-' + label + '-0', '360.ppm')
    image = Image.open(path).convert('RGB')
    ImageDraw.Draw(image).rectangle((900, 0, 1279, 119), fill=(0, 0, 0))
    return image


if __name__ == '__main__':
    entries = [
        ('core-audit-restored', True, 'Ordinary baseline Release; validation on'),
        ('core-audit-clean-long', True, 'Baseline probe; validation and implicit layers off'),
        ('optimized-cached-probe-long', True, 'Optimized probe; validation and implicit layers off'),
        ('optimized-native-validation', False, 'Native allocator correctness stress; validation on; not a benchmark'),
        ('optimized-clean-long', False, 'Overlapping demo captures; invalid performance comparison'),
        ('optimized-isolated-long', False, 'Old probe queried environment per draw; invalid shipping comparison'),
        ('optimized-final', True, 'Ordinary final Release; default validation off; implicit layers off')]
    before, after = quality('before'), quality('final')
    assert before.size == after.size == (1280, 720)
    difference = ImageChops.difference(before, after)
    result = {
        'verification_date': '2026-10-11',
        'backend': 'Windows Vulkan',
        'resolution': [1280, 720], 'residents': 200,
        'captures': [capture(*entry) for entry in entries],
        'quality': {'residents': 0, 'ocean_time': 3,
                    'ignored_hud_rectangle': [900, 0, 1280, 120],
                    'exact_scene_pixel_match': difference.getbbox() is None,
                    'difference_bbox': difference.getbbox()},
        'limitations': [
            'Private bytes are process memory, not VRAM.',
            'No overall GPU/frame or p99 improvement established at matching validation settings.',
            'Fixed-view pixel equivalence does not verify all cameras or other backends.',
            'Baseline and optimized ordinary Release validation defaults differ.']}
    target = os.path.join(ROOT, 'docs', 'performance', 'optimization-measurements.json')
    with open(target, 'w') as output:
        json.dump(result, output, indent=2, sort_keys=True, separators=(',', ': '))
        output.write('\n')
    print('Collected:', target)
    print('Exact scene pixel match:', result['quality']['exact_scene_pixel_match'])
    if not result['quality']['exact_scene_pixel_match']:
        raise SystemExit('Final fixed-view scene differs from baseline')
