import React from 'react';
import PropTypes from 'prop-types';
import { withRouter } from 'react-router-dom';

const CustomLink = ({ history, to, onClick, tag: Tag, staticContext, ...rest }) => {
    return <Tag
        {...rest}
        onClick={event => {
            onClick(event);
            history.push(to);
        }}
    />
};

CustomLink.propTypes = {
    to: PropTypes.string.isRequired,
    children: PropTypes.node.isRequired,
    history: PropTypes.shape({
        push: PropTypes.func.isRequired
    }).isRequired,
    onClick: PropTypes.func
};

CustomLink.defaultProps = {
    onClick: () => {}
};

export default withRouter(CustomLink);