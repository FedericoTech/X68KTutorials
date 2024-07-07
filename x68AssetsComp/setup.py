from setuptools import setup, find_packages

setup(
    name='x68AssetsComp',
    version='0.1.0',
    packages=find_packages(),
    install_requires=[
        # List your dependencies here
        'Pillow',  # Example dependency
    ],
    entry_points={
        'console_scripts': [
            'x68AssetsComp=main:main',
        ],
    },
    classifiers=[
        'Programming Language :: Python :: 3',
        'License :: OSI Approved :: MIT License',
        'Operating System :: OS Independent',
    ],
    python_requires='>=3.6',
)